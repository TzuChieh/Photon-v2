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
	m_producerThreadId = std::this_thread::get_id();

	m_thread.start();
}

template<typename Work>
template<typename DeducedWork>
inline void TSPSCExecutor<Work>::addWork(DeducedWork&& work)
{
	PH_ASSERT(isProducerThread());

	m_workQueue.enqueue(std::forward<DeducedWork>(work));
}

template<typename Work>
inline void TSPSCExecutor<Work>::waitAllWorks()
{
	PH_ASSERT(isProducerThread());

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

	terminate();
}

template<typename Work>
inline std::thread::id TSPSCExecutor<Work>::getId() const
{
	PH_ASSERT(m_thread.get_id() != std::thread::id());

	return m_thread.get_id();
}

template<typename Work>
inline void TSPSCExecutor<Work>::asyncProcessWork()
{
	PH_ASSERT(isWorkerThread());

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
	m_isTerminationRequested.test_and_set(std::memory_order_relaxed);

	addWork(Work(
		[this]()
		{
			m_isTerminationRequested.test_and_set(std::memory_order_relaxed);
		}));
}

}// end namespace ph
