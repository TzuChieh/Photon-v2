#pragma once

#include <Utility/IUninstantiable.h>
#include <Common/assertion.h>

#include <thread>

namespace ph::editor
{

class RenderThread;

class Threads final : private IUninstantiable
{
public:
	/*! @brief Whether current thread is the thread that called `main()`.
	@note Thread-safe in application main loop.
	*/
	static bool isOnMainThread();

	/*! @brief Whether current thread is the rendering thread.
	@note Thread-safe in application main loop.
	*/
	static bool isOnRenderThread();

	/*! @brief Whether current thread is the GHI thread.
	@note Thread-safe in application main loop.
	*/
	static bool isOnGHIThread();

	/*! @brief Get the rendering thread.
	It is an error to call this method when the application is not running.
	@note Thread-safe in application main loop.
	*/
	static RenderThread& getRenderThread();

private:
	friend class Program;
	friend class Application;

	static void setMainThreadID(std::thread::id threadID);
	static void setRenderThread(RenderThread* threadID);

	static std::thread::id mainThreadID;
	static std::thread::id renderThreadID;
	static std::thread::id ghiThreadID;
	static RenderThread* renderThread;

// Global 
private:
};

inline bool Threads::isOnMainThread()
{
	// Generally should not happen. Except being called outside the editor's domain which should be avoided.
	PH_ASSERT(mainThreadID != std::thread::id{});

	return std::this_thread::get_id() == mainThreadID;
}

inline bool Threads::isOnRenderThread()
{
	// May fail if called before the render thread has been properly initialized.
	// This can happen if some routine is attempting to use rendering functionalities before
	// the application starts running.
	PH_ASSERT(renderThreadID != std::thread::id{});

	return std::this_thread::get_id() == renderThreadID;
}

inline bool Threads::isOnGHIThread()
{
	// May fail if called before the GHI thread has been properly initialized.
	// This can happen if some routine is attempting to use GHI functionalities before
	// the application starts running.
	PH_ASSERT(ghiThreadID != std::thread::id{});

	return std::this_thread::get_id() == ghiThreadID;
}

inline RenderThread& Threads::getRenderThread()
{
	// May fail if called before the render thread has been properly initialized.
	// This can happen if some routine is attempting to use rendering functionalities before
	// the application starts running.
	PH_ASSERT(renderThreadID != std::thread::id{});
	PH_ASSERT(renderThread);

	return *renderThread;
}

}// end namespace ph::editor
