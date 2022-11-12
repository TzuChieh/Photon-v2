#pragma once

#include <Utility/IUninstantiable.h>
#include <Common/assertion.h>

#include <thread>

namespace ph::editor
{

class Threads final : private IUninstantiable
{
public:
	/*! @brief Whether current thread is the thread that called `main()`.
	*/
	static bool isOnMainThread();

	static bool isOnRenderThread();

private:
	friend class Program;
	friend class Application;

	static void setMainThreadID(std::thread::id threadID);
	static void setRenderThreadID(std::thread::id threadID);

	static std::thread::id mainThreadID;
	static std::thread::id renderThreadID;
};

inline bool Threads::isOnMainThread()
{
	// Generally should not happen. Except being called outside the editor's domain which should be avoided.
	PH_ASSERT(mainThreadID != std::thread::id());

	return std::this_thread::get_id() == mainThreadID;
}

inline bool Threads::isOnRenderThread()
{
	// May fail if called before the render thread has been properly initialized. This can happen if
	// some routine is attempting to use rendering functionalities before the application starts running.
	PH_ASSERT(renderThreadID != std::thread::id());

	return std::this_thread::get_id() == renderThreadID;
}

}// end namespace ph::editor
