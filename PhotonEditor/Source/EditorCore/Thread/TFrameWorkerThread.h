#pragma once

#include <Utility/INoCopyAndMove.h>
#include <Utility/MemoryArena.h>
#include <Utility/TFunction.h>
#include <Utility/Concurrent/TLockFreeQueue.h>
#include <Common/config.h>
#include <Common/assertion.h>
#include <Math/math.h>

#include <array>
#include <cstddef>
#include <thread>
#include <condition_variable>
#include <atomic>

namespace ph::editor
{

template<std::size_t NUM_BUFFERED_FRAMES, typename T>
class TFrameWorkerThread final
{
	// Correct function signature will instantiate the specialized type. If this type is selected
	// instead, notify the user about the ill-formed function signature
	static_assert(std::is_function_v<T>,
		"Invalid function signature.");
};

template<std::size_t NUM_BUFFERED_FRAMES, typename R, typename... Args>
class TFrameWorkerThread<NUM_BUFFERED_FRAMES, R(Args...)> final : private INoCopyAndMove
{
private:
	using Work = TFunction<R(Args...)>;

public:
	inline TFrameWorkerThread()
		: m_thread()
		, m_frameWorks()
		, m_isTerminationRequested(false)
		, m_parenThreadWorkHead(0)
		, m_workerThreadWorkHead(0)
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

	inline static std::size_t getNextWorkHead(const std::size_t currentWorkHead)
	{
		return math::wrap<std::size_t>(currentWorkHead + 1, 0, NUM_BUFFERED_FRAMES);
	}

	struct FrameWork
	{
		TLockFreeQueue<Work> workQueue;
		MemoryArena          workQueueMemory;
		std::atomic_flag     isFullyFilled;

		inline FrameWork()
			: workQueue()
			, workQueueMemory()
			, isFullyFilled()
		{}
	};

	std::array<FrameWork, NUM_BUFFERED_FRAMES + 1> m_frameWorks;

	std::thread      m_thread;
	std::atomic_bool m_isTerminationRequested;
	std::size_t      m_parenThreadWorkHead;
	std::size_t      m_workerThreadWorkHead;
};

}// end namespace ph::editor
