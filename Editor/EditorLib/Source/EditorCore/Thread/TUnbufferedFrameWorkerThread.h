#pragma once

#include <Utility/INoCopyAndMove.h>
#include <Utility/Concurrent/TSPSCExecutor.h>
#include <Utility/TFunction.h>
#include <Utility/MemoryArena.h>
#include <Common/assertion.h>

#include <utility>
#include <type_traits>
#include <cstddef>
#include <atomic>
#include <thread>

namespace ph::editor
{

template<typename T>
class TUnbufferedFrameWorkerThread
{
	// Correct function signature will instantiate the specialized type. If this type is selected
	// instead, notify the user about the ill-formed function signature
	static_assert(std::is_function_v<T>,
		"Invalid function signature.");
};

/*!
A worker thread that helps to develop the concept of frame-to-frame work which is executed on 
another thread. Unlike its variant, TFrameWorkerThread, this class has no buffer and works added
will be processed as soon as possible (no need to wait for the call to endFrame()).

Regarding thread safety notes:

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
*/
template<typename R, typename... Args>
class TUnbufferedFrameWorkerThread<R(Args...)> : private INoCopyAndMove
{
protected:
	using Work = TFunction<R(Args...)>;

public:
	struct FrameInfo final
	{
		std::size_t frameNumber                 = 0;
		std::size_t numParentWorks              = 0;
		std::size_t extraBytesAllocatedForWorks = 0;
	};

public:
	/*!
	Does not start the worker thread. Worker thread can be started by calling `startWorker()`.
	*/
	inline TUnbufferedFrameWorkerThread()
		: m_thread                   ()
		, m_workQueueMemory          ()
		, m_parentThreadId           ()
		, m_isStopRequested          (false)
		, m_frameNumber              (0)
		, m_numParentWorks           (0)
#ifdef PH_DEBUG
		, m_isBetweenFrameBeginAndEnd()
		, m_isStopped                (false)
#endif
	{}

	/*!
	Must call `waitForWorkerToStop()` before reaching dtor.
	*/
	inline virtual ~TUnbufferedFrameWorkerThread()
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
		PH_ASSERT(!m_isBetweenFrameBeginAndEnd);

		// The worker thread must not be already initiated. If this assertion fails, it is likely
		// that `startWorker()` is called more than once.
		PH_ASSERT(!hasWorkerStarted());

		m_parentThreadId = std::this_thread::get_id();

		m_thread.setWorkProcessor(
			[this](const Work& work)
			{
				onAsyncProcessWork(work);
			});

		m_thread.setOnConsumerStart(
			[this]()
			{
				onAsyncWorkerStart();
			});

		m_thread.setOnConsumerTerminate(
			[this]()
			{
				onAsyncWorkerStop();
			});

		m_thread.start();
	}

	/*!
	Start a new frame of works. All works added from previous frame are finished after this call.
	@note Parent thread only.
	*/
	inline void beginFrame()
	{
		PH_ASSERT(isParentThread());
		PH_ASSERT(hasWorkerStarted());

		// Wait until current frame is available for adding works
		m_thread.waitAllWorks();

		// Clear work queue memory after all works are done
		m_workQueueMemory.clear();

		// Works can now be added

#ifdef PH_DEBUG
		m_isBetweenFrameBeginAndEnd = true;
#endif

		onBeginFrame();
	}

	/*!
	End this frame.
	@note Parent thread only.
	*/
	inline void endFrame()
	{
		PH_ASSERT(isParentThread());
		PH_ASSERT(hasWorkerStarted());

		onEndFrame();

		// Works can no longer be added

#ifdef PH_DEBUG
		m_isBetweenFrameBeginAndEnd = false;
#endif

		m_numParentWorks = 0;
		++m_frameNumber;
	}

	/*!
	Similar to addWork(Work). This variant supports general functors. Larger functors or non-trivial
	functors may induce additional overhead on creating and processing of the work.
	@note Parent thread only.
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
	@note Parent thread only.
	*/
	inline void addWork(Work work)
	{
		PH_ASSERT(isParentThread());
		PH_ASSERT(m_isBetweenFrameBeginAndEnd);
		PH_ASSERT(work.isValid());

		m_thread.addWork(std::move(work));
		++m_numParentWorks;
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
		PH_ASSERT(m_isBetweenFrameBeginAndEnd);

		m_isStopRequested.store(true, std::memory_order_relaxed);

		// As we guarantee current frame must be completed before worker stopped, unfortunately we 
		// have to wait here otherwise we may cancel some work
		m_thread.waitAllWorks();

		// Request termination as early as possible since there are still some cleanups to do on
		// worker thread
		m_thread.requestTermination();

		// Clear work queue memory after all works are done
		m_workQueueMemory.clear();
	}

	/*!
	@note Thread-safe.
	*/
	inline bool isStopRequested() const
	{
		return m_isStopRequested.load(std::memory_order_relaxed);
	}

	/*!
	All operations on the worker thread will be done after this call returns. Can only be called 
	after `endFrame()`.
	@note Parent thread only.
	*/
	inline void waitForWorkerToStop()
	{
		PH_ASSERT(isParentThread());
		PH_ASSERT(!m_isBetweenFrameBeginAndEnd);
		PH_ASSERT(isStopRequested());
		PH_ASSERT(hasWorkerStarted());

		// Wait for cleanup operations to finish
		m_thread.waitForTermination();

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
	@note Parent thread only.
	*/
	inline FrameInfo getFrameInfo() const
	{
		PH_ASSERT(isParentThread());

		// Safe to access anywhere between being/end frame. Here just to be consistent with others
		PH_ASSERT(m_isBetweenFrameBeginAndEnd);

		FrameInfo info;
		info.frameNumber                 = getFrameNumber();
		info.numParentWorks              = m_numParentWorks;
		info.extraBytesAllocatedForWorks = m_workQueueMemory.numAllocatedBytes();

		return info;
	}

private:
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
	@note Parent thread only.
	*/
	template<typename Func>
	inline std::remove_reference_t<Func>* storeFuncInMemoryArena(Func&& workFunc)
	{
		PH_ASSERT(isParentThread());

		// No need to lock arena as it is essentially a single-thread resource (for parent thread only)
		return m_workQueueMemory.make<std::remove_reference_t<Func>>(std::forward<Func>(workFunc));
	}

	/*!
	@note Parent thread only.
	*/
	inline std::size_t getFrameNumber() const
	{
		PH_ASSERT(isParentThread());

		return m_frameNumber;
	}

	TSPSCExecutor<Work> m_thread;
	MemoryArena         m_workQueueMemory;
	std::thread::id     m_parentThreadId;
	std::atomic_bool    m_isStopRequested;
	std::size_t         m_frameNumber;
	std::size_t         m_numParentWorks;
#ifdef PH_DEBUG
	bool                m_isBetweenFrameBeginAndEnd;
	bool                m_isStopped;
#endif
};

}// end namespace ph::editor
