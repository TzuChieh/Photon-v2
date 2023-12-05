#include "Utility/Concurrent/TSPSCExecutor.h"

#include <Common/assertion.h>

namespace ph
{

template<typename Work>
inline TSPSCExecutor<Work>::~TSPSCExecutor()
{
	terminate();

	if(m_thread.hasStarted())
	{
		m_thread.join();
	}
}

template<typename Work>
inline void TSPSCExecutor<Work>::setWorkProcessor(std::function<void(const Work& work)> workProcessor)
{
	PH_ASSERT_MSG(!m_thread.hasStarted(),
		"cannot set the work processor after the executor has already started");

	m_workProcessor = std::move(workProcessor);
}

template<typename Work>
inline void TSPSCExecutor<Work>::setOnConsumerStart(std::function<void(void)> onConsumerStart)
{
	PH_ASSERT_MSG(!m_thread.hasStarted(),
		"cannot set callback after the executor has already started");

	m_onConsumerStart = std::move(onConsumerStart);
}

template<typename Work>
inline void TSPSCExecutor<Work>::setOnConsumerTerminate(std::function<void(void)> onConsumerTerminate)
{
	PH_ASSERT_MSG(!m_thread.hasStarted(),
		"cannot set callback after the executor has already started");

	m_onConsumerTerminate = std::move(onConsumerTerminate);
}

template<typename Work>
inline void TSPSCExecutor<Work>::start()
{
	PH_ASSERT_MSG(!m_thread.hasStarted(),
		"cannot start an already-started executor");
	PH_ASSERT_MSG(m_workProcessor,
		"work processor must be set before starting the executor");

	m_producerThreadId = std::this_thread::get_id();

	m_thread.start();
}

template<typename Work>
template<typename DeducedWork>
inline void TSPSCExecutor<Work>::addWork(DeducedWork&& work)
{
	static_assert(std::is_constructible_v<Work, DeducedWork>);

	PH_ASSERT(isProducerThread());
	PH_ASSERT(m_thread.hasStarted());

	m_workloadQueue.enqueue(std::forward<DeducedWork>(work));
}

template<typename Work>
inline void TSPSCExecutor<Work>::waitAllWorks()
{
	PH_ASSERT(isProducerThread());
	PH_ASSERT(m_thread.hasStarted());

	// As the work queue is FIFO when observed from any single thread, to wait all works added 
	// by this thread to finish, we add a new work and wait for it to finish (could be slower 
	// because we have one extra work to process, but generally good enough)

	std::atomic_flag isFinished;

	CustomCallable waitWork;
	waitWork.callable =
		[&isFinished]()
		{
			// Memory effects on consumer thread should be made visible
			isFinished.test_and_set(std::memory_order_release);
			isFinished.notify_one();
		};
	m_workloadQueue.enqueue(std::move(waitWork));

	// Wait for works added by this thread to finish; memory effects on consumer thread should be 
	// made visible
	isFinished.wait(false, std::memory_order_acquire);
}

template<typename Work>
inline void TSPSCExecutor<Work>::requestTermination()
{
	PH_ASSERT(isProducerThread());
	PH_ASSERT(m_thread.hasStarted());

	terminate();
}

template<typename Work>
inline void TSPSCExecutor<Work>::waitForTermination()
{
	PH_ASSERT_MSG(!isConsumerThread(),
		"wait for termination on consumer thread can lead to deadlock");

	PH_ASSERT_MSG(!isProducerThread() || (isProducerThread() && m_isTerminationRequested.test(std::memory_order_relaxed)),
		"termination not requested before wait on producer thread, this can lead to deadlock");

	// Memory effects on consumer thread should be made visible
	m_isTerminated.wait(false, std::memory_order_acquire);
}

template<typename Work>
inline std::thread::id TSPSCExecutor<Work>::getId() const
{
	PH_ASSERT(m_thread.getId() != std::thread::id());

	return m_thread.getId();
}

template<typename Work>
inline bool TSPSCExecutor<Work>::hasStarted() const
{
	return m_thread.hasStarted();
}

template<typename Work>
inline void TSPSCExecutor<Work>::asyncExecute()
{
	PH_ASSERT(isConsumerThread());
	PH_ASSERT(m_thread.hasStarted());

	if(m_onConsumerStart)
	{
		m_onConsumerStart();
	}

	asyncProcessWorks();

	if(m_onConsumerTerminate)
	{
		m_onConsumerTerminate();
	}

	// Memory effects before this should be made visible for waiting threads
	m_isTerminated.test_and_set(std::memory_order_release);
	m_isTerminated.notify_all();
}

template<typename Work>
inline void TSPSCExecutor<Work>::asyncProcessWorks()
{
	PH_ASSERT(isConsumerThread());
	PH_ASSERT(m_thread.hasStarted());
	PH_ASSERT(m_workProcessor);

	Workload currentWorkload;
	while(!m_isTerminationRequested.test(std::memory_order_relaxed))
	{
		m_workloadQueue.waitDequeue(&currentWorkload);

		if(std::holds_alternative<Work>(currentWorkload))
		{
			m_workProcessor(std::get<Work>(currentWorkload));
		}
		else if(std::holds_alternative<CustomCallable>(currentWorkload))
		{
			std::get<CustomCallable>(currentWorkload).callable();
		}
		else
		{
			PH_ASSERT(std::holds_alternative<std::monostate>(currentWorkload));
			PH_ASSERT_UNREACHABLE_SECTION();
		}
	}
}

template<typename Work>
inline bool TSPSCExecutor<Work>::isConsumerThread() const
{
	return getId() == std::this_thread::get_id();
}

template<typename Work>
inline bool TSPSCExecutor<Work>::isProducerThread() const
{
	PH_ASSERT(m_producerThreadId != std::thread::id());

	return std::this_thread::get_id() == m_producerThreadId;
}

template<typename Work>
inline void TSPSCExecutor<Work>::terminate()
{
	// Nothing to terminate if the executor was not even started. Basically querying this state is safe as
	// it is either called on producer thread or on dtor calling thread, and the contract is that the 
	// programmer should ensure a proper ordering of `start()` -> `requestTermination()` -> `dtor()`.
	if(!m_thread.hasStarted())
	{
		return;
	}

	// Just try to break the worker loop as soon as possible (and as non-intrusive as possible)
	m_isTerminationRequested.test_and_set(std::memory_order_relaxed);

	// If the consumer is waiting or is going to wait, adding another work that sets the flag is 
	// required (so the consumer can unwait). The flag will be set on the consumer thread and is 
	// guaranteed to break the loop there as testing the flag is sequentially after setting 
	// the flag.
	CustomCallable terminateWork;
	terminateWork.callable =
		[this]()
		{
			m_isTerminationRequested.test_and_set(std::memory_order_relaxed);
		};
	m_workloadQueue.enqueue(std::move(terminateWork));
}

}// end namespace ph
