#pragma once

#include <Utility/INoCopyAndMove.h>
#include <Utility/MemoryArena.h>
#include <Utility/TFunction.h>
#include <Utility/Concurrent/TAtomicQueue.h>
#include <Common/config.h>
#include <Common/assertion.h>
#include <Math/math.h>
#include <Common/logging.h>
#include <Utility/Concurrent/InitiallyPausedThread.h>

#include <array>
#include <cstddef>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <mutex>
#include <utility>
#include <type_traits>
#include <vector>

namespace ph::editor
{

PH_DEFINE_EXTERNAL_LOG_GROUP(TFrameWorkerThread, EditorCore);

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
* Worker Thread: Thread that processes/consumes work, only one worker thread will be spawned.
* Parent Thread: Thread that starts the worker thread (by calling `startWorker()`).
* Thread Safe: Can be used on any thread.
* Anything that has no thread safety notes: It is **NOT** thread safe.

A typical call sequence of the class would be like:

* ctor call
* `startWorker()`
* one or more calls to:
  - `beginFrame()`
  - any number of calls to `addWork()`
  - possibly call `requestWorkerStop()`
  - `endFrame()`
* `waitForWorkerToStop()`
* dtor call

It is the programmer's responsibility to make sure the class instance outlive the worker's full
working cycle--from `startWorker()` to `waitForWorkerToStop()`. Ctor and dtor calls are not thread safe.

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
	// Safe limit of concurrent works processed to avoid starvation on parent thread works
	inline constexpr static std::size_t maxAnyThreadWorksPerFrame = 16384;

	struct Frame final
	{
		std::vector<Work>   parentThreadWorkQueue;
		TAtomicQueue<Work>  anyThreadWorkQueue;
		MemoryArena         workQueueMemory;
		mutable std::mutex  memoryMutex;
		std::atomic_flag    isSealedForProcessing;
#ifdef PH_DEBUG
		std::atomic_int32_t numAnyThreadWorks;
		bool                isBetweenFrameBeginAndEnd;
#endif

		inline Frame()
			: parentThreadWorkQueue    ()
			, anyThreadWorkQueue       ()
			, workQueueMemory          ()
			, memoryMutex              ()
			, isSealedForProcessing    ()
#ifdef PH_DEBUG
			, numAnyThreadWorks        (0)
			, isBetweenFrameBeginAndEnd(false)
#endif
		{}
	};

public:
	struct FrameInfo final
	{
		std::size_t frameNumber     = 0;
		std::size_t frameCycleIndex = 0;
		std::size_t numParentWorks  = 0;

		struct Detail final
		{
			std::size_t numEstimatedAnyThreadWorks  = 0;
			std::size_t numEstimatedTotalWorks      = 0;
			std::size_t estimatedLocalBytesForWorks = 0;
			std::size_t extraBytesAllocatedForWorks = 0;

			inline std::size_t estimatedTotalBytesUsed() const
			{
				return estimatedLocalBytesForWorks + extraBytesAllocatedForWorks + sizeof(TFrameWorkerThread);
			}
		};

		Detail detail;
		bool hasDetail = false;
	};

public:
	/*!
	Does not start the worker thread. Worker thread can be started by calling `startWorker()`.
	*/
	inline TFrameWorkerThread()
		: m_thread              ()
		, m_frames              ()
		, m_isStopRequested     (false)
		, m_workProducerWorkHead(0)
		, m_workConsumerWorkHead(0)
		, m_frameNumber         (0)
#ifdef PH_DEBUG
		, m_parentThreadId      ()
		, m_isStopped           (false)
#endif
	{
		m_thread = InitiallyPausedThread(
			[this]()
			{
				asyncProcessFrame();
			});
	}

	/*!
	Must call `waitForWorkerToStop()` before reaching dtor.
	*/
	inline virtual ~TFrameWorkerThread()
	{
		// Do not call `waitForWorkerToStop()` here as the worker sould finish its work before 
		// reaching dtor to ensure `onAsyncProcessWork()` will not be called after parent object 
		// has already destructed. This assertion make sure this does not happen.
		PH_ASSERT(m_isStopped);
	}

	/*! @brief Called when each work is extracted and is going to be processed.
	@param work The work to be processed. How to process it depends on the implementation.
	@note Called on worker thread only.
	*/
	virtual void onAsyncProcessWork(const Work& work) = 0;

	/*! @brief Called when the worker has started.
	@note Called on worker thread only.
	*/
	inline virtual void onAsyncWorkerStart()
	{}

	/*! @brief Called when the worker has stopped.
	@note Called on worker thread only.
	*/
	inline virtual void onAsyncWorkerStop()
	{}

	/*! @brief Called right after the frame begins.
	@note Called on parent thread only.
	*/
	inline virtual void onBeginFrame()
	{}

	/*! @brief Called right before the frame ends.
	@note Called on parent thread only.
	*/
	inline virtual void onEndFrame()
	{}

	/*! @brief Start the worker.
	Call to this method establishes synchronizes-with the execution of the worker thread. This method
	should not be called in the ctor as the worker thread will call virtual methods internally, 
	which leads to possible UB if any derived class has not been initialized yet.
	@note The thread that calls this method is considered the worker's parent thread.
	*/
	inline void startWorker()
	{
		PH_ASSERT(!getCurrentProducerFrame().isBetweenFrameBeginAndEnd);

		// The worker thread must not be already initiated. If this assertion fails, it is likely
		// that `startWorker()` is called more than once.
		PH_ASSERT(!hasWorkerStarted());

		m_parentThreadId = std::this_thread::get_id();
		m_thread.start();
	}

	/*!
	@note Parent thread only.
	*/
	inline void beginFrame()
	{
		PH_ASSERT(isParentThread());
		PH_ASSERT(hasWorkerStarted());

		// Wait until current frame is available for adding works
		Frame& currentFrame = getCurrentProducerFrame();
		currentFrame.isSealedForProcessing.wait(true, std::memory_order_acquire);

		//--------------------------------------------------------------------//
		// vvvvv   Works can now be added after the acquire operation   vvvvv //
		//--------------------------------------------------------------------//
		
		// Must be empty (already cleared by worker)
		PH_ASSERT_EQ(currentFrame.numAnyThreadWorks.load(std::memory_order_relaxed), 0);

#ifdef PH_DEBUG
		getCurrentProducerFrame().isBetweenFrameBeginAndEnd = true;
#endif

		onBeginFrame();
	}

	/*!
	@note Parent thread only.
	*/
	inline void endFrame()
	{
		PH_ASSERT(isParentThread());
		PH_ASSERT(hasWorkerStarted());

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
	Similar to addWork(Work). This variant supports general functors. Larger functors or non-trivial
	functors may induce additional overhead on creating and processing of the work.
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
	Can only be called after the frame begins and before the frame ends, i.e., between calls to 
	`beginFrame()` and `endFrame()`. Additionally, calling from frame callbacks such as `onBeginFrame()`
	and `onEndFrame()` is also allowed.
	@note Producer threads only.
	*/
	inline void addWork(Work work)
	{
		PH_ASSERT(getCurrentProducerFrame().isBetweenFrameBeginAndEnd);
		PH_ASSERT(work.isValid());

		Frame& currentFrame = getCurrentProducerFrame();
		if(isParentThread())
		{
			currentFrame.parentThreadWorkQueue.push_back(std::move(work));
		}
		else
		{
			PH_ASSERT(!isWorkerThread());

			currentFrame.anyThreadWorkQueue.enqueue(std::move(work));

#ifdef PH_DEBUG
			currentFrame.numAnyThreadWorks.fetch_add(1, std::memory_order_relaxed);
#endif
		}
	}

	/*! @brief Ask the worker thread to stop.
	The worker thread will stop as soon as possible. Currently processing frame will still complete
	before the worker stop. Can only be called between `beginFrame()` and `endFrame()`.
	@note Parent thread only.
	*/
	inline void requestWorkerStop()
	{
		PH_ASSERT(isParentThread());

		// Ensures that `endFrame()` will be called later, which guarantees that the stop condition
		// will be checked (in case of the worker was already waiting, `endFrame()` will unwait it)
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

		m_thread.join();

#ifdef PH_DEBUG
		m_isStopped = true;
#endif
	}

	/*!
	@note Thread-safe.
	*/
	inline std::thread::id getWorkerThreadId() const
	{
		// ID is always valid.
		PH_ASSERT(m_thread.getId() != std::thread::id());

		return m_thread.getId();
	}

	/*!
	@note Producer threads only.
	*/
	inline FrameInfo getFrameInfo(const bool shouldIncludeDetails = false) const
	{
		// For all producer threads, it is only safe to access between being/end frame.
		PH_ASSERT(!isWorkerThread());
		PH_ASSERT(getCurrentProducerFrame().isBetweenFrameBeginAndEnd);

		const Frame& currentFrame = getCurrentProducerFrame();

		FrameInfo info;
		info.frameNumber     = getFrameNumber();
		info.frameCycleIndex = m_workProducerWorkHead;
		info.numParentWorks  = currentFrame.parentThreadWorkQueue.size();

		if(shouldIncludeDetails)
		{
			info.detail.numEstimatedAnyThreadWorks  = currentFrame.anyThreadWorkQueue.estimatedSize();
			info.detail.numEstimatedTotalWorks      = info.numParentWorks + info.detail.numEstimatedAnyThreadWorks;
			info.detail.estimatedLocalBytesForWorks = info.detail.numEstimatedTotalWorks * sizeof(Work);

			{
				std::lock_guard<std::mutex> lock(currentFrame.memoryMutex);

				info.detail.extraBytesAllocatedForWorks = currentFrame.workQueueMemory.numAllocatedBytes();
			}

			info.hasDetail = true;
		}

		return info;
	}

private:
	/*!
	@note Worker thread only.
	*/
	inline void asyncProcessFrame()
	{
		PH_ASSERT(isWorkerThread());

		onAsyncWorkerStart();

		do
		{
			// Wait until being notified there is new frame that can be processed
			Frame& currentFrame = getCurrentConsumerFrame();
			currentFrame.isSealedForProcessing.wait(false, std::memory_order_acquire);

			//------------------------------------------------------------------------//
			// vvvvv   Works can now be processed after the acquire operation   vvvvv //
			//------------------------------------------------------------------------//

			// Process works added from parent thread
			{
				for(const Work& workFromParentThread : currentFrame.parentThreadWorkQueue)
				{
					onAsyncProcessWork(workFromParentThread);
				}
				currentFrame.parentThreadWorkQueue.clear();
			}

			// Process works added from any thread
			{
				Work workFromAnyThread;
				std::size_t numDequeuedAnyThreadWorks = 0;
				while(currentFrame.anyThreadWorkQueue.tryDequeue(&workFromAnyThread))
				{
					++numDequeuedAnyThreadWorks;

					onAsyncProcessWork(workFromAnyThread);

					if(numDequeuedAnyThreadWorks == maxAnyThreadWorksPerFrame + 1)
					{
						PH_LOG_WARNING(TFrameWorkerThread,
							"too many concurrently added works ({}), current safe limit is {}",
							numDequeuedAnyThreadWorks + currentFrame.anyThreadWorkQueue.estimatedSize(), maxAnyThreadWorksPerFrame);
					}

#ifdef PH_DEBUG
					currentFrame.numAnyThreadWorks.fetch_sub(1, std::memory_order_relaxed);
#endif
				}
			}

			// Since there is no consumer contention, there should be no work once `tryDequeue` fails 
			PH_ASSERT_EQ(currentFrame.numAnyThreadWorks.load(std::memory_order_relaxed), 0);

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

		onAsyncWorkerStop();
	}

	/*! @brief Check if this thread is worker thread.
	@note Thread-safe.
	*/
	inline bool isWorkerThread() const
	{
		return std::this_thread::get_id() == m_thread.getId();
	}

	/*! @brief Check if this thread is parent thread.
	@note Thread-safe.
	*/
	inline bool isParentThread() const
	{
		return std::this_thread::get_id() == m_parentThreadId;
	}

#ifdef PH_DEBUG
	/*! @brief Check whether the worker thread has started.
	@note Thread-safe.
	*/
	inline bool hasWorkerStarted() const
	{
		return m_thread.hasStarted();
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

	InitiallyPausedThread m_thread;
	std::atomic_bool      m_isStopRequested;
	std::size_t           m_workProducerWorkHead;
	std::size_t           m_workConsumerWorkHead;
	std::size_t           m_frameNumber;
#ifdef PH_DEBUG
	std::thread::id       m_parentThreadId;
	bool                  m_isStopped;
#endif
};

}// end namespace ph::editor
