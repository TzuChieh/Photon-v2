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

template<std::size_t NUM_BUFFERS, typename T>
class TFrameWorkerThread
{
	// Correct function signature will instantiate the specialized type. If this type is selected
	// instead, notify the user about the ill-formed function signature
	static_assert(std::is_function_v<T>,
		"Invalid function signature.");
};

/*!
Regarding thread safety notes:
* Producer Thread: Thread that adds/produces work, can be one or more.
* Worker Thread: Thread that processes/consumes work, only one worker thread is allowed.
* Parent Thread: Thread that creates an instance of this class.
* Thread Safe: Can be used on any thread.
@tparam NUM_BUFFERS Number of buffered frames. For example, 1 corresponds to single buffering which
forces the worker to wait until the work producer is done; 2 corresponds to double buffering where
the worker and the work producer may have their own buffer (frame) to work on. Any number of buffers
are supported.
*/
template<std::size_t NUM_BUFFERS, typename R, typename... Args>
class TFrameWorkerThread<NUM_BUFFERS, R(Args...)> : private INoCopyAndMove
{
	static_assert(NUM_BUFFERS >= 1,
		"Must have at least 1 buffer.");

protected:
	using Work = TFunction<R(Args...)>;

private:
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
	/*!
	@note The thread that creates the worker is considered the worker's parent thread.
	*/
	inline TFrameWorkerThread()
		: m_thread()
		, m_frames()
		, m_isStopRequested(false)
		, m_workProducerWorkHead(0)
		, m_workConsumerWorkHead(0)
		, m_frameNumber(0)
#ifdef PH_DEBUG
		, m_parentThreadId(std::this_thread::get_id())
		, m_isStopped(false)
#endif
	{
		m_thread = std::thread([this]()
		{
			asyncProcessFrame();
		});
	}

	/*!
	@note Parent thread only.
	*/
	inline virtual ~TFrameWorkerThread()
	{
		// Do not call `waitForWorkerToStop()` here as the worker sould finish its work before 
		// reaching dtor to ensure `onAsyncProcessWork()` will not be called after parent object 
		// has already destructed. This assertion make sure this does not happen.
		PH_ASSERT(m_isStopped);
	}

	/*!
	@note Called on worker thread only.
	*/
	virtual void onAsyncProcessWork(const Work& work) = 0;

	/*!
	@note Called on parent thread only.
	*/
	virtual void onBeginFrame(std::size_t frameNumber) = 0;

	/*!
	@note Called on parent thread only.
	*/
	virtual void onEndFrame() = 0;

	/*!
	@note Parent thread only.
	*/
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

		onBeginFrame(getFrameNumber());
	}

	/*!
	@note Parent thread only.
	*/
	inline void endFrame()
	{
		PH_ASSERT(isParentThread());

		onEndFrame();

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
		++m_frameNumber;
	}

	/*!
	Can only be called between `beginFrame()` and `endFrame()`.
	@note Producer threads only.
	*/
	template<typename Func>
	inline void addWork(Func&& workFunc)
	{
		if constexpr(Work::template TIsStorableFunctor<Func>::value)
		{
			addWork(Work(std::forward<Func>(workFunc)));
		}
		else
		{
			// If `Func` cannot be stored in the internal buffer of `TFunction`, then
			// we allocate its space in the arena and call it through a wrapper

			auto* const funcPtr = storeFuncInMemoryArena(std::forward<Func>(workFunc));
			PH_ASSERT(funcPtr);

			addWork(Work(
				[funcPtr](Args... args) -> R
				{
					return (*funcPtr)(std::forward<Args>(args)...);
				}));
		}
	}

	/*!
	Can only be called between `beginFrame()` and `endFrame()`.
	@note Producer threads only.
	*/
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

	/*!
	Can only be called between `beginFrame()` and `endFrame()`.
	@note Parent thread only.
	*/
	inline void requestWorkerStop()
	{
		PH_ASSERT(isParentThread());
		PH_ASSERT(getCurrentProducerFrame().isBetweenFrameBeginAndEnd);

		m_isStopRequested.store(true, std::memory_order_relaxed);
	}

	/*!
	@note Thread-safe.
	*/
	inline bool isStopRequested() const
	{
		return m_isStopRequested.load(std::memory_order_relaxed);
	}

	/*!
	Can only be called after `endFrame()`.
	@note Parent thread only.
	*/
	inline void waitForWorkerToStop()
	{
		PH_ASSERT(isParentThread());
		PH_ASSERT(!getCurrentProducerFrame().isBetweenFrameBeginAndEnd);
		PH_ASSERT(isStopRequested());

		// Ensures that `endFrame()` is called at least once before requesting stop, so worker can process 
		// at least one frame and see the stop request (rather than block forever).
		PH_ASSERT_GE(getFrameNumber(), 1);

		if(m_thread.joinable())
		{
			m_thread.join();
		}

#ifdef PH_DEBUG
		m_isStopped = true;
#endif
	}

	/*!
	@note Thread-safe.
	*/
	inline std::thread::id getWorkerThreadId() const
	{
		return m_thread.get_id();
	}

private:
	/*!
	@note Worker thread only.
	*/
	inline void asyncProcessFrame()
	{
		PH_ASSERT(isWorkerThread());

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
				onAsyncProcessWork(work);

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

	/*!
	@note Producer threads only.
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

	/*!
	@note Producer threads only.
	*/
	///@{
	inline const Frame& getCurrentProducerFrame() const
	{
		PH_ASSERT(!isWorkerThread());
		return m_frames[m_workProducerWorkHead];
	}

	inline Frame& getCurrentProducerFrame()
	{
		PH_ASSERT(!isWorkerThread());
		return m_frames[m_workProducerWorkHead];
	}
	///@}

	/*!
	@note Worker thread only.
	*/
	///@{
	inline const Frame& getCurrentConsumerFrame() const
	{
		PH_ASSERT(isWorkerThread());
		return m_frames[m_workConsumerWorkHead];
	}

	inline Frame& getCurrentConsumerFrame()
	{
		PH_ASSERT(isWorkerThread());
		return m_frames[m_workConsumerWorkHead];
	}
	///@}

	/*!
	@note Parent thread only.
	*/
	inline void advanceCurrentProducerFrame()
	{
		PH_ASSERT(isParentThread());
		m_workProducerWorkHead = getNextWorkHead(m_workProducerWorkHead);
	}

	/*!
	@note Worker thread only.
	*/
	inline void advanceCurrentConsumerFrame()
	{
		PH_ASSERT(isWorkerThread());
		m_workConsumerWorkHead = getNextWorkHead(m_workConsumerWorkHead);
	}

	/*!
	@note Producer threads only.
	*/
	inline std::size_t getFrameNumber() const
	{
		// For all producer threads, it is only safe to access between being/end frame.
		// If it is parent thread, it is safe to access anytime.
		PH_ASSERT(
			(!isWorkerThread() && getCurrentProducerFrame().isBetweenFrameBeginAndEnd) ||
			(isParentThread()));

		return m_frameNumber;
	}

	/*!
	@note Thread-safe.
	*/
	inline static std::size_t getNextWorkHead(const std::size_t currentWorkHead)
	{
		return math::wrap<std::size_t>(currentWorkHead + 1, 0, NUM_BUFFERS - 1);
	}

	std::array<Frame, NUM_BUFFERS> m_frames;

	std::thread      m_thread;
	std::atomic_bool m_isStopRequested;
	std::size_t      m_workProducerWorkHead;
	std::size_t      m_workConsumerWorkHead;
	std::size_t      m_frameNumber;
#ifdef PH_DEBUG
	std::thread::id  m_parentThreadId;
	bool             m_isStopped;
#endif
};

}// end namespace ph::editor
