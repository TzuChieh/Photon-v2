#pragma once

#include <Utility/INoCopyAndMove.h>
#include <Utility/MemoryArena.h>
#include <Utility/TFunction.h>
#include <Utility/Concurrent/TAtomicQuasiQueue.h>
#include <Common/config.h>
#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/primitive_type.h>
#include <Utility/Concurrent/InitiallyPausedThread.h>
#include <Utility/Concurrent/TSPSCRingBuffer.h>
#include <Utility/Concurrent/TSynchronized.h>

#include <cstddef>
#include <thread>
#include <atomic>
#include <utility>
#include <type_traits>
#include <vector>

namespace ph::editor
{

PH_DEFINE_EXTERNAL_LOG_GROUP(TFrameWorkerThread, EditorCore);

struct BufferedFrameInfo final
{
	uint64      frameNumber        = 0;
	uint32      frameCycleIndex    = 0;
	std::size_t numParentWorks     = 0;
	std::size_t sizeofWorkerThread = 0;

	struct Detail final
	{
		std::size_t numEstimatedAnyThreadWorks  = 0;
		std::size_t numEstimatedTotalWorks      = 0;
		std::size_t estimatedLocalBytesForWorks = 0;
		std::size_t extraBytesAllocatedForWorks = 0;
	};

	Detail detail;
	bool hasDetail = false;

	inline std::size_t estimatedTotalBytesUsed() const
	{
		return detail.estimatedLocalBytesForWorks + 
		       detail.extraBytesAllocatedForWorks + 
		       sizeofWorkerThread;
	}
};

template<std::size_t NUM_BUFFERS, typename T>
class TFrameWorkerThread
{
	// Correct function signature will instantiate the specialized type. If this type is selected
	// instead, notify the user about the ill-formed function signature
	static_assert(std::is_function_v<T>,
		"Invalid function signature.");
};

/*!
A worker thread that helps to develop the concept of frame-to-frame work which is executed on
another thread. For unbuffered frames, you can specify `NUM_BUFFERS = 1`, though one might consider
to use `TUnbufferedFrameWorkerThread` for this as it is specialized for this case and can offer better
performance. Note that in `TFrameWorkerThread`, works will not be processed until `endFrame()` is called, 
while in `TUnbufferedFrameWorkerThread` works will start being processed right after `beginFrame()`.

Regarding thread safety notes:

* Parent Thread: Thread that starts the worker thread (by calling `startWorker()`).
* Worker Thread: Thread that processes/consumes work, only one worker thread will be spawned.
* Producer Thread: Thread that adds/produces work, can be one or more. For a non-parent producer thread,
it must synchronize with `beginFrame()` and `endFrame()`.
* Thread Safe: Can be used on any thread.
* Anything that has no thread safety notes: It is **NOT** thread safe.

A typical call sequence of the class would be like:

* ctor call
* `startWorker()`
* one or more calls to:
  - `beginFrame()`
  - any number of calls to `addWork()`
  - possibly call `requestWorkerStop()` or `waitAllWorks()`
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

public:
	using Work = TFunction<R(Args...)>;

private:
	// Safe limit of concurrent works processed to avoid starvation on parent thread works
	inline constexpr static std::size_t maxAnyThreadWorksPerFrame = 16384;

	struct Frame final
	{
		std::vector<Work>          parentThreadWorkQueue;
		TAtomicQuasiQueue<Work>    anyThreadWorkQueue;
		TSynchronized<MemoryArena> workQueueMemory;
#if PH_DEBUG
		std::atomic_int32_t        numAnyThreadWorks;
#endif

		inline Frame()
			: parentThreadWorkQueue()
			, anyThreadWorkQueue   ()
			, workQueueMemory      ()
#if PH_DEBUG
			, numAnyThreadWorks    (0)
#endif
		{}
	};

public:
	inline TFrameWorkerThread()
		: TFrameWorkerThread(true)
	{}

	/*!
	Does not start the worker thread. Worker thread can be started by calling `startWorker()`.
	@param shouldFlushBufferBeforeStop Whether all submitted works in the remaining frames should be
	processed before actually stopping the worker.
	*/
	inline TFrameWorkerThread(bool shouldFlushBufferBeforeStop)
		: m_thread()
		, m_frames()
		, m_isStopRequested()
		, m_hasFinalFrameEnded()
		, m_frameNumber(0)
		, m_shouldFlushBufferBeforeStop(shouldFlushBufferBeforeStop)
		, m_parentThreadId()
#if PH_DEBUG
		, m_isBetweenFrameBeginAndEnd(false)
		, m_isStopped(false)
#endif
	{
		m_thread = InitiallyPausedThread(
			[this]()
			{
				asyncProcessFrames();
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
	Call to this method synchronizes-with the execution of the worker thread. This method
	should not be called in the ctor as the worker thread will call virtual methods internally, 
	which leads to possible UB if any derived class has not been initialized yet.
	@note The thread that calls this method is considered the worker's parent thread.
	*/
	inline void startWorker()
	{
		PH_ASSERT(!m_frames.isProducing());

		// The worker thread must not be already initiated. If this assertion fails, it is likely
		// that `startWorker()` is called more than once.
		PH_ASSERT(!hasWorkerStarted());

		m_parentThreadId = std::this_thread::get_id();
		m_thread.start();
	}

	/*!
	Start a new frame of works. All works added from previous frame are either being processing or 
	finished after this call (depending on NUM_BUFFERS). Will potentially wait for works to complete.
	@note Parent thread only.
	*/
	inline void beginFrame()
	{
		PH_ASSERT(isParentThread());
		PH_ASSERT(hasWorkerStarted());
		PH_ASSERT(!isStopRequested());

		// Wait until current frame is available for adding works
		m_frames.beginProduce();
		Frame& currentFrame = m_frames.getBufferForProducer();

		//--------------------------------------------------------------------//
		// vvvvv   Works can now be added after the begin produce call  vvvvv //
		//--------------------------------------------------------------------//
		
#if PH_DEBUG
		m_isBetweenFrameBeginAndEnd = true;
#endif

		// Must be empty (already cleared by worker)
		PH_ASSERT_EQ(currentFrame.numAnyThreadWorks.load(std::memory_order_relaxed), 0);

		onBeginFrame();
	}

	/*!
	End this frame and works are submitted to the worker for processing.
	@note Parent thread only.
	*/
	inline void endFrame()
	{
		PH_ASSERT(isParentThread());
		PH_ASSERT(hasWorkerStarted());

		onEndFrame();

#if PH_DEBUG
		m_isBetweenFrameBeginAndEnd = false;
#endif

		// Mark that current frame is now filled and is ready for processing
		m_frames.endProduce();

		//--------------------------------------------------------------------------//
		// ^^^^^   Works can no longer be added after the end produce call    ^^^^^ //
		//--------------------------------------------------------------------------//

		if(isStopRequested())
		{
			// Ensure memory effects of `m_frames` will be visible on worker
			m_hasFinalFrameEnded.test_and_set(std::memory_order_release);
			m_hasFinalFrameEnded.notify_one();
		}

		++m_frameNumber;
	}

	/*!
	Similar to addWork(Work). This variant supports general functors. Larger functors or non-trivial
	functors will induce additional overhead on creating and processing of the work. If you want to 
	ensure minimal overhead, adhere to the binding requirements imposed by `TFunction`.
	@note Producer threads only. Work objects will be destructed (if required) on worker thread.
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

	/*! @brief Adds a work that will be executed on the worker thread.
	Can only be called after the frame begins and before the frame ends, i.e., between calls to 
	`beginFrame()` and `endFrame()`. Additionally, calling from frame callbacks such as `onBeginFrame()`
	and `onEndFrame()` is also allowed.
	@note Producer threads only.
	*/
	inline void addWork(Work work)
	{
		PH_ASSERT(work.isValid());
		PH_ASSERT(m_isBetweenFrameBeginAndEnd);

		// Unsafe buffer reference is okay--producer threads should synchronize with frame begin/end
		Frame& currentFrame = m_frames.unsafeGetBufferReference(m_frames.unsafeGetProduceHead());
		if(isParentThread())
		{
			currentFrame.parentThreadWorkQueue.push_back(std::move(work));
		}
		else
		{
			PH_ASSERT(!isWorkerThread());

			currentFrame.anyThreadWorkQueue.enqueue(std::move(work));

#if PH_DEBUG
			currentFrame.numAnyThreadWorks.fetch_add(1, std::memory_order_relaxed);
#endif
		}
	}

	/*! @brief Ask the worker thread to stop.
	The worker thread will stop as soon as possible. Currently processing frame will still complete
	before the worker stop. Whether works in buffered frames will be executed or not depends on the
	value of `shouldFlushBufferedWorksBeforeStop` specified to ctor. Can only be called between 
	`beginFrame()` and `endFrame()`. Works can still be added after this call.
	@note Parent thread only. This is a request only, no memory effect is implied.
	*/
	inline void requestWorkerStop()
	{
		PH_ASSERT(isParentThread());

		// Ensures that `endFrame()` will be called later, which guarantees that the stop condition
		// will be checked (in case of the worker was already waiting, `endFrame()` will unwait it)
		PH_ASSERT(m_frames.isProducing());

		m_isStopRequested.test_and_set(std::memory_order_relaxed);
	}

	/*!
	@note Thread-safe.
	*/
	inline bool isStopRequested() const
	{
		return m_isStopRequested.test(std::memory_order_relaxed);
	}

	/*!
	All operations on the worker thread will be done after this call returns. Can only be called 
	after `endFrame()`.
	@note Parent thread only.
	*/
	inline void waitForWorkerToStop()
	{
		PH_ASSERT(isParentThread());
		PH_ASSERT(!m_frames.isProducing());
		PH_ASSERT(isStopRequested());
		PH_ASSERT(hasWorkerStarted());

		// Ensures that `endFrame()` is called at least once before requesting stop, so worker can
		// process at least one frame and see the stop request (rather than block forever).
		PH_ASSERT_GE(getFrameNumber(), 1);

		m_thread.join();

#if PH_DEBUG
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
	Get information about current frame. Can only be called between `beginFrame()` and `endFrame()`.
	@param shouldIncludeDetails Flag for filling the detailed frame information, which incurs 
	non-negligible overhead comparing to not filling.
	@note Producer threads only.
	*/
	inline BufferedFrameInfo getFrameInfo(const bool shouldIncludeDetails = false) const
	{
		// For all producer threads, it is only safe to access between being/end frame.
		PH_ASSERT(!isWorkerThread());
		PH_ASSERT(m_isBetweenFrameBeginAndEnd);

		// Unsafe buffer reference is okay--producer threads should synchronize with frame begin/end
		const Frame& currentFrame = m_frames.unsafeGetBufferReference(m_frames.unsafeGetProduceHead());

		BufferedFrameInfo info;
		info.frameNumber        = getFrameNumber();
		info.frameCycleIndex    = static_cast<uint32>(m_frames.getProduceHead());
		info.numParentWorks     = currentFrame.parentThreadWorkQueue.size();
		info.sizeofWorkerThread = sizeof(*this);

		if(shouldIncludeDetails)
		{
			info.detail.numEstimatedAnyThreadWorks  = currentFrame.anyThreadWorkQueue.estimatedSize();
			info.detail.numEstimatedTotalWorks      = info.numParentWorks + info.detail.numEstimatedAnyThreadWorks;
			info.detail.estimatedLocalBytesForWorks = info.detail.numEstimatedTotalWorks * sizeof(Work);
			info.detail.extraBytesAllocatedForWorks = currentFrame.workQueueMemory->numAllocatedBytes();

			info.hasDetail = true;
		}

		return info;
	}

private:
	/*!
	@note Worker thread only.
	*/
	inline void asyncProcessFrames()
	{
		PH_ASSERT(isWorkerThread());

		onAsyncWorkerStart();

		do
		{
			// Wait until being notified there is new frame that can be processed
			m_frames.beginConsume();
			Frame& currentFrame = m_frames.getBufferForConsumer();

			//------------------------------------------------------------------------//
			// vvvvv   Works can now be processed after the begin consume call  vvvvv //
			//------------------------------------------------------------------------//

			processSingleFrame(currentFrame);

			//------------------------------------------------------------------------------//
			// ^^^^^   Works can no longer be processed after the end consume call    ^^^^^ //
			//------------------------------------------------------------------------------//

			// Mark that current frame is now processed and is ready for producer again
			m_frames.endConsume();

		} while(!isStopRequested());

		// Ensure memory effects of `m_frames` from parent will be visible. After this, contents in
		// `m_frames` are for worker thread to use exclusively.
		m_hasFinalFrameEnded.wait(false, std::memory_order_acquire);

		const std::size_t numRemainingFrames = m_frames.getHeadDistanceDirectly();
		for(std::size_t i = 0; i < numRemainingFrames; ++i)
		{
			const auto consumerHead = m_frames.nextProducerConsumerHead(m_frames.getConsumeHead(), i);
			Frame& remainingFrame = m_frames.unsafeGetBufferReference(consumerHead);

			// Process all remaining works
			if(m_shouldFlushBufferBeforeStop)
			{
				processSingleFrame(remainingFrame);
			}
			// Perform cleanup directly
			else
			{
				remainingFrame.parentThreadWorkQueue.clear();
				remainingFrame.anyThreadWorkQueue = TAtomicQuasiQueue<Work>{};
#if PH_DEBUG
				remainingFrame.numAnyThreadWorks.store(0, std::memory_order_relaxed);
#endif
				remainingFrame.workQueueMemory.unsafeGetReference().clear();
			}
		}

		onAsyncWorkerStop();
	}

	inline void processSingleFrame(Frame& frame)
	{
		PH_ASSERT(isWorkerThread());

		// Process works added from parent thread
		{
			for(const Work& workFromParentThread : frame.parentThreadWorkQueue)
			{
				onAsyncProcessWork(workFromParentThread);
			}
			frame.parentThreadWorkQueue.clear();
		}

		// Process works added from any thread
		{
			Work workFromAnyThread;
			std::size_t numDequeuedAnyThreadWorks = 0;
			while(frame.anyThreadWorkQueue.tryDequeue(&workFromAnyThread))
			{
				++numDequeuedAnyThreadWorks;

				onAsyncProcessWork(workFromAnyThread);

				if(numDequeuedAnyThreadWorks == maxAnyThreadWorksPerFrame + 1)
				{
					PH_LOG_WARNING(TFrameWorkerThread,
						"too many concurrently added works ({}), current safe limit is {}",
						numDequeuedAnyThreadWorks + frame.anyThreadWorkQueue.estimatedSize(), maxAnyThreadWorksPerFrame);
				}

#if PH_DEBUG
				frame.numAnyThreadWorks.fetch_sub(1, std::memory_order_relaxed);
#endif
			}
		}

		// Since there is no consumer contention, there should be no work once `tryDequeue` fails 
		PH_ASSERT_EQ(frame.numAnyThreadWorks.load(std::memory_order_relaxed), 0);

		// It is worker's job to cleanup this frame for producer (for performance reasons; 
		// also, some code is only meant to be called on the worker thread, this includes
		// non-trivial destructors).
		// No need to lock the arena here as `isSealedForProcessing` already established
		// proper ordering for the memory effects to be visible.
		frame.workQueueMemory.unsafeGetReference().clear();
	}

	/*! @brief Check if this thread is worker thread.
	@note Thread-safe.
	*/
	inline bool isWorkerThread() const
	{
		return std::this_thread::get_id() == getWorkerThreadId();
	}

	/*! @brief Check if this thread is parent thread.
	@note Thread-safe.
	*/
	inline bool isParentThread() const
	{
		PH_ASSERT(m_parentThreadId != std::thread::id());

		return std::this_thread::get_id() == m_parentThreadId;
	}

#if PH_DEBUG
	/*! @brief Check whether the worker thread has started.
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
		PH_ASSERT(m_isBetweenFrameBeginAndEnd);

		// Unsafe buffer reference is okay--producer threads should synchronize with frame begin/end
		TSynchronized<MemoryArena>& currentArena = 
			m_frames.unsafeGetBufferReference(m_frames.unsafeGetProduceHead()).workQueueMemory;

		// Since it is possible that multiple threads are concurrently storing oversized functor,
		// we simply lock the arena on allocation to avoid contention
		return currentArena->make<std::remove_reference_t<Func>>(std::forward<Func>(workFunc));
	}

	/*!
	@note Producer threads only.
	*/
	inline uint64 getFrameNumber() const
	{
		// For all producer threads, it is only safe to access between being/end frame.
		// If it is parent thread, it is safe to access anytime.
		PH_ASSERT(
			(!isWorkerThread() && m_isBetweenFrameBeginAndEnd) ||
			(isParentThread()));

		return m_frameNumber;
	}

	TSPSCRingBuffer<Frame, NUM_BUFFERS> m_frames;

	InitiallyPausedThread m_thread;
	std::atomic_flag      m_isStopRequested;
	std::atomic_flag      m_hasFinalFrameEnded;
	uint64                m_frameNumber;
	bool                  m_shouldFlushBufferBeforeStop;
	std::thread::id       m_parentThreadId;
#if PH_DEBUG
	bool                  m_isBetweenFrameBeginAndEnd;
	bool                  m_isStopped;
#endif
};

}// end namespace ph::editor
