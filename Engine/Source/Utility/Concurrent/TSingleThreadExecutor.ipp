#include "Utility/Concurrent/TSingleThreadExecutor.h"
#include "Common/assertion.h"

namespace ph
{

template<typename Work>
inline TSingleThreadExecutor<Work>::~TSingleThreadExecutor()
{
	requestTermination();

	if(m_thread.joinable())
	{
		m_thread.join();
	}
}

template<typename Work>
template<typename DeducedWork>
inline void TSingleThreadExecutor<Work>::addWork(DeducedWork&& work)
{
	m_workQueue.enqueue(std::forward<DeducedWork>(work));
}

template<typename Work>
inline void TSingleThreadExecutor<Work>::waitAllWorksFromThisThread()
{
	// As the work queue is FIFO when observed from any single thread, to wait all works added by this
	// thread to finish, we add a new work and wait for it to finish (could be slower because we have
	// one extra work to process, but generally good enough)

	bool isFinished = false;
	addWork(Work(
		[this, &isFinished]()
		{
			{
				std::lock_guard<std::mutex> lock(m_executorMutex);

				isFinished = true;
			}

			// Notify the caller thread which is waiting on `isFinished`
			m_waitWorksCv.notify_all();
		}));

	// Wait for works added by this thread to finish (by checking `isFinished`). The wait can be ended
	// early if termination is requested. `m_numWaitingThreads` is updated accordingly so `requestTermination()`
	// can be sure that no one is waiting.
	{
		std::unique_lock<std::mutex> lock(m_executorMutex);

		++m_numWaitingThreads;

		m_waitWorksCv.wait(lock, 
			[this, &isFinished]()
			{
				return isFinished || m_isTerminationRequested.test(std::memory_order_relaxed);
			});

		PH_ASSERT_GT(m_numWaitingThreads, 0);
		--m_numWaitingThreads;

		if(m_numWaitingThreads == 0)
		{
			// Notify `requestTermination()` that `m_numWaitingThreads == 0`
			m_waitWorksCv.notify_all();
		}
	}
}

// FIXME: this method can race with waitAllWorksFromThisThread(), might return early if 
// waitAllWorksFromThisThread() is called on another thread and yielded halfway
template<typename Work>
inline void TSingleThreadExecutor<Work>::requestTermination()
{
	addWork(Work(
		[this]()
		{
			m_isTerminationRequested.test_and_set(std::memory_order_relaxed);
		}));

	{
		std::lock_guard<std::mutex> lock(m_executorMutex);

		m_isTerminationRequested.test_and_set(std::memory_order_relaxed);
	}

	// Notify anyone waiting on termination
	m_waitWorksCv.notify_all();

	{
		std::unique_lock<std::mutex> lock(m_executorMutex);

		m_waitWorksCv.wait(lock,
			[this]()
			{
				return m_numWaitingThreads == 0;
			});
	}
}

template<typename Work>
inline std::thread::id TSingleThreadExecutor<Work>::getId() const
{
	PH_ASSERT(m_thread.get_id() != std::thread::id());

	return m_thread.get_id();
}

template<typename Work>
inline void TSingleThreadExecutor<Work>::asyncProcessWork()
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
inline bool TSingleThreadExecutor<Work>::isWorkerThread() const
{
	return getId() == std::this_thread::get_id();
}

}// end namespace ph
