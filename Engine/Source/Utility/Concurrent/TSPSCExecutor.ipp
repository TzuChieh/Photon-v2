#include "Utility/Concurrent/TSPSCExecutor.h"
#include "Common/assertion.h"

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
inline void TSPSCExecutor<Work>::start()
{
	PH_ASSERT(!m_thread.hasStarted());

	m_producerThreadId = std::this_thread::get_id();

	m_thread.start();
}

template<typename Work>
template<typename DeducedWork>
inline void TSPSCExecutor<Work>::addWork(DeducedWork&& work)
{
	PH_ASSERT(isProducerThread());
	PH_ASSERT(m_thread.hasStarted());

	m_workQueue.enqueue(std::forward<DeducedWork>(work));
}

template<typename Work>
inline void TSPSCExecutor<Work>::waitAllWorks()
{
	PH_ASSERT(isProducerThread());
	PH_ASSERT(m_thread.hasStarted());

	// As the work queue is FIFO when observed from any single thread, to wait all works added by this
	// thread to finish, we add a new work and wait for it to finish (could be slower because we have
	// one extra work to process, but generally good enough)

	std::atomic_flag isFinished;
	addWork(Work(
		[this, &isFinished]()
		{
			isFinished.test_and_set(std::memory_order_relaxed);
			isFinished.notify_one();
		}));

	// Wait for works added by this thread to finish
	isFinished.wait(false, std::memory_order_relaxed);
}

template<typename Work>
inline void TSPSCExecutor<Work>::requestTermination()
{
	PH_ASSERT(isProducerThread());
	PH_ASSERT(m_thread.hasStarted());

	terminate();
}

template<typename Work>
inline std::thread::id TSPSCExecutor<Work>::getId() const
{
	PH_ASSERT(m_thread.getId() != std::thread::id());

	return m_thread.getId();
}

template<typename Work>
inline void TSPSCExecutor<Work>::asyncProcessWork()
{
	PH_ASSERT(isConsumerThread());
	PH_ASSERT(m_thread.hasStarted());

	while(!m_isTerminationRequested.test(std::memory_order_relaxed))
	{
		Work& currentWork = m_defaultWork;
		m_workQueue.waitDequeue(&currentWork);

		currentWork();
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

	// If the consumer as waiting, adding another work that sets the flag is required. The flag will be set on
	// the consumer thread and is guaranteed to break the loop there as testing the flag is sequentially after
	// setting the flag.
	addWork(Work(
		[this]()
		{
			m_isTerminationRequested.test_and_set(std::memory_order_relaxed);
		}));
}

}// end namespace ph
