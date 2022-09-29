#pragma once

#include <Utility/INoCopyAndMove.h>
#include <Utility/MemoryArena.h>
#include <Utility/TFunction.h>
#include <Utility/Concurrent/TLockFreeQueue.h>
#include <Common/config.h>
#include <Common/assertion.h>

#include <array>
#include <cstddef>
#include <thread>
#include <condition_variable>
#include <atomic>

#ifdef PH_DEBUG
#include <atomic>
#endif

namespace ph::editor
{

template<std::size_t N, typename T>
class TFrameWorkerThread final
{
	// Correct function signature will instantiate the specialized type. If this type is selected
	// instead, notify the user about the ill-formed function signature
	static_assert(std::is_function_v<T>,
		"Invalid function signature.");
};

template<std::size_t N, typename R, typename... Args>
class TFrameWorkerThread<N, R(Args...)> final : private INoCopyAndMove
{
private:
	using Work = TFunction<R(Args...)>;

public:
	inline TFrameWorkerThread()
		: m_thread()
		, m_threadMutex()
		, m_processFrameCv()
		, m_isTerminationRequested(false)
		, m_workQueues()
		, m_workQueueMemories()
		, m_currentQueueIdx(0)
		, m_processingQueueIdx(0)
#ifdef PH_DEBUG
		, m_queueSizes()
#endif
	{
		// TODO
	}

	inline ~TFrameWorkerThread()
	{

	}

	inline void beginFrameWork()
	{
		PH_ASSERT(!isWorkerThread());

		MemoryArena& memory = getCurrentWorkQueueMemory();


		// TODO
	}

	inline void endFrameWork()
	{
		PH_ASSERT(!isWorkerThread());

		// TODO
	}

	inline void queueFrameWork()
	{
		PH_ASSERT(!isWorkerThread());

		// TODO
	}

	inline void requestWorkerStop()
	{
		PH_ASSERT(!isWorkerThread());

		// TODO
	}

	inline TLockFreeQueue<Work>& getCurrentWorkQueue()
	{
		return m_workQueues[m_currentQueueIdx];
	}

	inline MemoryArena& getCurrentWorkQueueMemory()
	{
		return m_workQueueMemories[m_currentQueueIdx];
	}

private:
	/*! @brief
	@note Thread-safe.
	*/
	inline void asyncProcessWork()
	{
		std::unique_lock<std::mutex> lock(m_threadMutex);

		do
		{
			// Wait until being notified there is new frame that can be processed
			m_processFrameCv.wait(lock, [this]()
			{
				return !m_works.empty() || m_isTerminationRequested;
			});

			// We now own the lock after waiting
			if(!m_works.empty() && !m_isTerminationRequested)
			{
				Work work = std::move(m_works.front());
				m_works.pop();

				// We are done using the work queue
				lock.unlock();

				work();

				// Current thread must own the lock before calling wait(2)
				lock.lock();

				PH_ASSERT_GT(m_numUnfinishedWorks, m_works.size());

				--m_numUnfinishedWorks;
				if(m_numUnfinishedWorks == 0)
				{
					m_allWorksDoneCv.notify_all();
				}
			}
		} while(!m_isTerminationRequested);
	}

	/*! @brief 
	@note Thread-safe.
	*/
	inline bool isWorkerThread() const
	{
		return std::this_thread::get_id() == m_thread.get_id();
	}

	std::thread                             m_thread;
	std::mutex                              m_threadMutex;
	std::condition_variable                 m_processFrameCv;
	bool                                    m_isTerminationRequested;
	std::array<TLockFreeQueue<Work>, N + 1> m_workQueues;
	std::array<MemoryArena, N + 1>          m_workQueueMemories;
	std::size_t                             m_currentQueueIdx;
	std::size_t                             m_processingQueueIdx;

#ifdef PH_DEBUG
	std::array<std::atomic_uint32_t, N + 1> m_queueSizes;
#endif
};

}// end namespace ph::editor
