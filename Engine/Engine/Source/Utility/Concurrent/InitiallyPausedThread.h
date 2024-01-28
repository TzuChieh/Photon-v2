#pragma once

#include "Utility/IMoveOnly.h"

#include <Common/debug.h>
#include <Common/exception.h>
#include <Common/logging.h>

#include <thread>
#include <future>
#include <functional>
#include <utility>

namespace ph
{

class InitiallyPausedThread final : private IMoveOnly
{
public:
	/*! @brief An empty object that does not represent a thread.
	*/
	InitiallyPausedThread();

	inline InitiallyPausedThread(InitiallyPausedThread&& other) = default;

	/*! @brief Creates a thread with a function to execute.
	The function will not execute immediately. The thread is in paused state initially and the execution
	can be started by calling start().
	@param func Functor to be called by the thread. It is copied/moved by value and stored in the thread.
	@param args Arguments passed to the functor. They are copied/moved by value and stored in the thread.
	@note @p func and @p args are passed the same way as `std::thread`, which means to pass a reference
	argument to the thread, it has to be wrapped (e.g., with `std::ref` or `std::cref`).
	*/
	template<typename Func, typename... Args>
	explicit InitiallyPausedThread(Func&& func, Args&&... args);

	/*!
	join() must be called before the thread destructs.
	*/
	~InitiallyPausedThread();

	/*! @brief Start the execution of the thread.
	This method synchronizes-with the start of the call to the functor.
	@exception IllegalOperationException If any error occurred.
	*/
	void start();

	/*! @brief Wait until the execution of the thread is finished.
	hasStarted() must be true when calling this method. The finish of the call to the functor 
	synchronizes-with this method.
	@exception IllegalOperationException If any error occurred.
	*/
	void join();

	/*! @brief Whether the thread has started executing the functor.
	The started state is visible to the body of the functor.
	*/
	bool hasStarted() const;

	/*! @brief Whether the thread has joined.
	*/
	bool hasJoined() const;

	/*! @brief Get ID of the underlying thread.
	@return Thread ID. Will be empty (`std::thread::id()`) if this class is default constructed.
	*/
	std::thread::id getId() const;

	inline InitiallyPausedThread& operator = (InitiallyPausedThread&& rhs) = default;

private:
	void setPromisedValue(bool value);
	bool isEmptyThread() const;

	std::thread        m_thread;
	std::promise<bool> m_startPromise;
	bool               m_hasStarted;
	bool               m_hasJoined;
};

template<typename Func, typename... Args>
inline InitiallyPausedThread::InitiallyPausedThread(Func&& func, Args&&... args)
	: InitiallyPausedThread()
{
	m_thread = std::thread(
		[startFuture = m_startPromise.get_future(),
		 func = std::bind(std::forward<Func>(func), std::forward<Args>(args)...)]
		() mutable
		{
			try
			{
				if(startFuture.get())
				{
					func();
				}
			}
			catch(const Exception& e)
			{
				PH_DEFAULT_LOG(Error,
					"[InitiallyPausedThread] unhandled exception thrown: {}", e.what());

				PH_DEBUG_BREAK();
			}
		});
}

inline bool InitiallyPausedThread::hasStarted() const
{
	return m_hasStarted;
}

inline bool InitiallyPausedThread::hasJoined() const
{
	return m_hasJoined;
}

inline std::thread::id InitiallyPausedThread::getId() const
{
	return m_thread.get_id();
}

inline bool InitiallyPausedThread::isEmptyThread() const
{
	return m_thread.get_id() == std::thread::id();
}

}// end namespace ph
