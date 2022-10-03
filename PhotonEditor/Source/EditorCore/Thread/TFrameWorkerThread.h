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
#include <mutex>
#include <utility>
#include <type_traits>

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

	struct Frame final
	{
		TLockFreeQueue<Work> workQueue;
		MemoryArena          workQueueMemory;
		std::mutex           memoryMutex;
		std::atomic_flag     isSealedForProcessing;
#ifdef PH_DEBUG
		std::atomic_int32_t  numWorks;
		bool                 isBetweenFrameBeginAndEnd;
#endif

		inline Frame()
			: workQueue()
			, workQueueMemory()
			, memoryMutex()
			, isSealedForProcessing()
#ifdef PH_DEBUG
			, numWorks(0)
			, isBetweenFrameBeginAndEnd(false)
#endif
		{}
	};

public:
	inline TFrameWorkerThread()
		: m_thread()
		, m_frames()
		, m_isStopRequested(false)
		, m_workProducerWorkHead(0)
		, m_workConsumerWorkHead(0)
#ifdef PH_DEBUG
		, m_parentThreadId(std::this_thread::get_id())
#endif
	{
		// TODO
	}

	inline ~TFrameWorkerThread()
	{
		PH_ASSERT(isParentThread());
		PH_ASSERT(!getCurrentProducerFrame().isBetweenFrameBeginAndEnd);

		// TODO
	}

	virtual void asyncProcessWork(const Work& work) = 0;

	inline void beginFrame()
	{
		PH_ASSERT(isParentThread());

		// Wait until current frame is available for adding works
		Frame& currentFrame = getCurrentProducerFrame();
		currentFrame.isSealedForProcessing.wait(true, std::memory_order_acquire);

		//--------------------------------------------------------------------//
		// vvvvv   Works can now be added after the acquire operation   vvvvv //
		//--------------------------------------------------------------------//
		
		// Must be empty (already cleared by worker)
		PH_ASSERT_EQ(currentFrame.numWorks.load(std::memory_order_relaxed), 0);

#ifdef PH_DEBUG
		getCurrentProducerFrame().isBetweenFrameBeginAndEnd = true;
#endif
	}

	inline void endFrame()
	{
		PH_ASSERT(isParentThread());

#ifdef PH_DEBUG
		getCurrentProducerFrame().isBetweenFrameBeginAndEnd = false;
#endif

		// Mark that current frame is now filled and is ready for processing
		Frame& currentFrame = getCurrentProducerFrame();
		const bool hasAlreadySealed = currentFrame.isSealedForProcessing.test_and_set(std::memory_order_release);
		PH_ASSERT(!hasAlreadySealed);

		//--------------------------------------------------------------------------//
		// ^^^^^   Works can no longer be added after the release operation   ^^^^^ //
		//--------------------------------------------------------------------------//

		// Notify the potentially waiting worker so it can start processing this frame
		currentFrame.isSealedForProcessing.notify_one();

		advanceCurrentProducerFrame();
	}

	template<typename Func>
	inline void addWork(Func&& workFunc)
	{
		if constexpr(Work::template TCanFitBuffer<Func>{})
		{
			addWork(Work(std::forward<Func>(workFunc)));
		}
		else
		{
			// If `Func` is too large to be stored in the internal buffer of `TFunction`, then
			// we allocate its space in the arena and call it with a wrapper

			auto* const funcPtr = storeFuncInMemoryArena(std::forward<Func>(workFunc));
			PH_ASSERT(funcPtr);

			addWork(Work(
				[funcPtr](Args... args) -> R
				{
					return (*funcPtr)(std::forward<Args>(args)...);
				}));
		}
	}

	inline void addWork(Work work)
	{
		PH_ASSERT(!isWorkerThread());
		PH_ASSERT(getCurrentProducerFrame().isBetweenFrameBeginAndEnd);
		PH_ASSERT(work.isValid());

		Frame& currentFrame = getCurrentProducerFrame();
		currentFrame.workQueue.enqueue(std::move(work));

#ifdef PH_DEBUG
		currentFrame.numWorks.fetch_add(1, std::memory_order_relaxed);
#endif
	}

	inline void requestWorkerStop()
	{
		PH_ASSERT(isParentThread());
		PH_ASSERT(getCurrentProducerFrame().isBetweenFrameBeginAndEnd);

		m_isStopRequested.store(true, std::memory_order_relaxed);
	}

private:
	/*! @brief
	@note Thread-safe.
	*/
	inline void asyncProcessFrame()
	{
		do
		{
			// Wait until being notified there is new frame that can be processed
			Frame& currentFrame = getCurrentConsumerFrame();
			currentFrame.isSealedForProcessing.wait(false, std::memory_order_acquire);

			//------------------------------------------------------------------------//
			// vvvvv   Works can now be processed after the acquire operation   vvvvv //
			//------------------------------------------------------------------------//

			Work work;
			while(currentFrame.workQueue.tryDequeue(&work))
			{
				asyncProcessWork(work);

#ifdef PH_DEBUG
				currentFrame.numWorks.fetch_sub(1, std::memory_order_relaxed);
#endif
			}

			// Since there is no consumer contention, there should be no work once `tryDequeue` fails 
			PH_ASSERT_EQ(currentFrame.numWorks.load(std::memory_order_relaxed), 0);

			// It is worker's job to cleanup this frame for producer (for performance reasons).
			// No need to lock `memoryMutex` here as `isSealedForProcessing` already established
			// proper ordering for the memory effects to be visible
			currentFrame.workQueueMemory.clear();

			//------------------------------------------------------------------------------//
			// ^^^^^   Works can no longer be processed after the release operation   ^^^^^ //
			//------------------------------------------------------------------------------//

			currentFrame.isSealedForProcessing.clear(std::memory_order_release);

			// Notify the potentially waiting producer (parent thread) so it can start adding work
			currentFrame.isSealedForProcessing.notify_one();

			advanceCurrentConsumerFrame();
		} while(!isStopRequested());
	}

	/*! @brief Check if this thread is worker thread.
	@note Thread-safe.
	*/
	inline bool isWorkerThread() const
	{
		return std::this_thread::get_id() == m_thread.get_id();
	}

#ifdef PH_DEBUG
	/*! @brief Check if this thread is parent thread.
	@note Thread-safe.
	*/
	inline bool isParentThread() const
	{
		return std::this_thread::get_id() == m_parentThreadId;
	}
#endif

	/*! @brief
	@note Thread-safe.
	*/
	inline bool isStopRequested() const
	{
		PH_ASSERT(isWorkerThread());
		return m_isStopRequested.load(std::memory_order_relaxed);
	}

	/*! @brief
	@note Thread-safe.
	*/
	template<typename Func>
	inline std::remove_reference_t<Func>* storeFuncInMemoryArena(Func&& workFunc)
	{
		PH_ASSERT(!isWorkerThread());

		// Since it is possible that multiple threads are concurrently storing oversized functor,
		// we simply lock the arena since such operation is thread-unsafe
		std::lock_guard<std::mutex> lock(getCurrentProducerFrame().memoryMutex);

		MemoryArena& currentArena = getCurrentProducerFrame().workQueueMemory;
		return currentArena.make<std::remove_reference_t<Func>>(std::forward<Func>(workFunc));
	}

	inline Frame& getCurrentProducerFrame()
	{
		PH_ASSERT(!isWorkerThread());
		return m_frames[m_workProducerWorkHead];
	}

	inline Frame& getCurrentConsumerFrame()
	{
		PH_ASSERT(isWorkerThread());
		return m_frames[m_workConsumerWorkHead];
	}

	inline void advanceCurrentProducerFrame()
	{
		PH_ASSERT(!isWorkerThread());
		m_workProducerWorkHead = getNextWorkHead(m_workProducerWorkHead);
	}

	inline void advanceCurrentConsumerFrame()
	{
		PH_ASSERT(isWorkerThread());
		m_workConsumerWorkHead = getNextWorkHead(m_workConsumerWorkHead);
	}

	inline static std::size_t getNextWorkHead(const std::size_t currentWorkHead)
	{
		return math::wrap<std::size_t>(currentWorkHead + 1, 0, NUM_BUFFERED_FRAMES);
	}

	std::array<Frame, NUM_BUFFERED_FRAMES + 1> m_frames;

	std::thread      m_thread;
	std::atomic_bool m_isStopRequested;
	std::size_t      m_workProducerWorkHead;
	std::size_t      m_workConsumerWorkHead;
#ifdef PH_DEBUG
	std::thread::id  m_parentThreadId;
#endif
};

}// end namespace ph::editor