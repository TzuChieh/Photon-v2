#include "Utility/Concurrent/InitiallyPausedThread.h"

#include <Common/assertion.h>
#include <Common/exceptions.h>

#include <stdexcept>

namespace ph
{

InitiallyPausedThread::InitiallyPausedThread()
	: m_thread      ()
	, m_startPromise()
	, m_hasStarted  (false)
	, m_hasJoined   (false)
{}

void InitiallyPausedThread::start()
{
	PH_ASSERT_MSG(!isEmptyThread(),
		"cannot start a default-constructed thread");
	PH_ASSERT_MSG(!m_hasStarted,
		"cannot start an already started thread");
	PH_ASSERT_MSG(!m_hasJoined,
		"cannot restart an already joined thread");

	// Set started flag **before** promised value, so the future side can see it after waiting is done
	m_hasStarted = true;

	// Set to `true` to run the work
	setPromisedValue(true);
}

void InitiallyPausedThread::join()
{
	PH_ASSERT_MSG(!isEmptyThread(),
		"cannot join a default-constructed thread");
	PH_ASSERT_MSG(m_hasStarted,
		"cannot join a thread that has not started already (this can lead to deadlock)");
	PH_ASSERT_MSG(!m_hasJoined,
		"cannot join an already joined thread");

	if(m_thread.joinable())
	{
		m_thread.join();
		m_hasJoined = true;
	}
}

void InitiallyPausedThread::setPromisedValue(const bool value)
{
	try
	{
		m_startPromise.set_value(value);
	}
	catch(const std::future_error& e)
	{
		throw_formatted<IllegalOperationException>(
			"Error occurred when starting the thread: {}",
			e.what());
	}
}

InitiallyPausedThread::~InitiallyPausedThread()
{
	// The only valid states when a thread destructs are:
	// 1. The thread is empty, so it neither has started nor has joined.
	// 2. The thread is non-empty but has never started, so it neither has started nor has joined.
	// 3. The thread is non-empty and has started and joined.
	// The above conditions lead to the following assert (1. and 2. have the same state)
	PH_ASSERT_MSG(
		(m_hasStarted && m_hasJoined) ||
		(!m_hasStarted && !m_hasJoined),
		"invalid thread method call order detected");

	// Unblock the waiting thread if we never started its execution
	if(!isEmptyThread() && !m_hasStarted)
	{
		// Set to `false` so it will not run the work
		setPromisedValue(false);

		if(m_thread.joinable())
		{
			m_thread.join();
		}
	}
}

}// end namespace ph
