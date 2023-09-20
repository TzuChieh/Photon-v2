#pragma once

#include <Utility/INoCopyAndMove.h>
#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Utility/Concurrent/TSPSCExecutor.h>
#include <Utility/TFunction.h>
#include <Utility/MemoryArena.h>

#include <utility>
#include <type_traits>
#include <cstddef>
#include <atomic>
#include <thread>
#include <variant>

namespace ph::editor
{

struct UnbufferedFrameInfo final
{
	uint64      frameNumber                 = 0;
	std::size_t numParentWorks              = 0;
	std::size_t sizeofWorkerThread          = 0;
	std::size_t extraBytesAllocatedForWorks = 0;
};

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
another thread. Unlike its variant, `TFrameWorkerThread`, this class has no buffer and works added
will be processed as soon as possible (no need to wait for the call to endFrame()).

Regarding thread safety notes:

* Parent Thread: Thread that starts the worker thread (by calling `startWorker()`).
* Worker Thread: Thread that processes/consumes work, only one worker thread will be spawned.
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
public:
	using Work = TFunction<R(Args...)>;

private:
	// Work type for internal usages. Warpping with a custom private type so we cannot mix 
	// it with user types
	struct InternalWork
	{
		TFunction<void(void), 0> callable;
	};

	// Possibly store both user-specified work and custom callables for internal usages
	using Workload = std::variant<
		std::monostate,
		Work,
		InternalWork>;

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
#if PH_DEBUG
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
			[this](const Workload& workload)
			{
				if(std::holds_alternative<Work>(workload))
				{
					onAsyncProcessWork(std::get<Work>(workload));
				}
				else if(std::holds_alternative<InternalWork>(workload))
				{
					std::get<InternalWork>(workload).callable();
				}
				else
				{
					PH_ASSERT(std::holds_alternative<std::monostate>(workload));
					PH_ASSERT_UNREACHABLE_SECTION();
				}
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
	Start a new frame of works. All works added from previous frame are finished after this call and 
	memory effects are made visible to the parent thread.
	@note Parent thread only.
	*/
	inline void beginFrame()
	{
		PH_ASSERT(isParentThread());
		PH_ASSERT(hasWorkerStarted());
		PH_ASSERT(!isStopRequested());

		// Wait until current frame is available for adding works, this includes clearing the work 
		// queue memory (arena)
		m_thread.waitAllWorks();

		// Works can now be added

#if PH_DEBUG
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

		// Clear work queue memory (arena) on worker thread after all works are added
		addClearArenaWork();

		// Works can no longer be added

#if PH_DEBUG
		m_isBetweenFrameBeginAndEnd = false;
#endif

		if(isStopRequested())
		{
			// As we guarantee current frame must be completed before worker stopped, 
			// unfortunately we have to wait here otherwise we may cancel some work
			m_thread.waitAllWorks();

			// Request termination as early as possible since there are still some cleanups 
			// to do on worker thread
			m_thread.requestTermination();
		}

		m_numParentWorks = 0;
		++m_frameNumber;
	}

	/*!
	Similar to addWork(Work). This variant supports general functors. Larger functors or non-trivial
	functors will induce additional overhead on creating and processing of the work. If you want to 
	ensure minimal overhead, adhere to the binding requirements imposed by `TFunction`.
	@note Parent thread only. Work objects will be destructed (if required) on worker thread.
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

	/*! @brief Wait for all added works to finish.
	Can only be called after the frame begins and before the frame ends, i.e., between calls to
	`beginFrame()` and `endFrame()`. Additionally, calling from frame callbacks such as `onBeginFrame()`
	and `onEndFrame()` is also allowed. Memory effects on worker thread are made visible to the parent thread.
	@note Parent thread only.
	*/
	inline void waitAllWorks()
	{
		PH_ASSERT(isParentThread());
		PH_ASSERT(m_isBetweenFrameBeginAndEnd);

		m_thread.waitAllWorks();
	}

	/*! @brief Ask the worker thread to stop.
	The worker thread will stop as soon as possible. Currently processing frame will still complete
	before the worker stop. Can only be called between `beginFrame()` and `endFrame()`. Works can
	still be added after this call.
	@note Parent thread only. This is a request only, no memory effect is implied.
	*/
	inline void requestWorkerStop()
	{
		PH_ASSERT(isParentThread());

		// Ensures that `endFrame()` will be called later, some cleanup works are only done there
		PH_ASSERT(m_isBetweenFrameBeginAndEnd);

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
	@note Parent thread only.
	*/
	inline UnbufferedFrameInfo getFrameInfo() const
	{
		PH_ASSERT(isParentThread());

		// Safe to access anywhere between being/end frame. Here just to be consistent with others
		PH_ASSERT(m_isBetweenFrameBeginAndEnd);

		UnbufferedFrameInfo info;
		info.frameNumber                 = getFrameNumber();
		info.numParentWorks              = m_numParentWorks;
		info.sizeofWorkerThread          = sizeof(*this);
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

#if PH_DEBUG
	/*! @brief Check whether the worker thread has started.
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

		// No need to lock arena as it is essentially a single-thread resource (for parent thread only).
		// Clearing the arena is done on worker thread however--care must be taken when doing that.
		return m_workQueueMemory.make<std::remove_reference_t<Func>>(std::forward<Func>(workFunc));
	}

	/*!
	@note Parent thread only.
	*/
	inline void addClearArenaWork()
	{
		PH_ASSERT(isParentThread());
		PH_ASSERT(m_isBetweenFrameBeginAndEnd);

		InternalWork clearArenaWork;
		clearArenaWork.callable = 
			[this]()
			{
				m_workQueueMemory.clear();
			};
		m_thread.addWork(std::move(clearArenaWork));
	}

	/*!
	@note Parent thread only.
	*/
	inline uint64 getFrameNumber() const
	{
		PH_ASSERT(isParentThread());

		return m_frameNumber;
	}

	TSPSCExecutor<Workload>    m_thread;
	MemoryArena                m_workQueueMemory;
	std::thread::id            m_parentThreadId;
	std::atomic_bool           m_isStopRequested;
	uint64                     m_frameNumber;
	std::size_t                m_numParentWorks;
#if PH_DEBUG
	bool                       m_isBetweenFrameBeginAndEnd;
	bool                       m_isStopped;
#endif
};

}// end namespace ph::editor
