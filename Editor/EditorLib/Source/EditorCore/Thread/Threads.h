#pragma once

#include <Utility/IUninstantiable.h>
#include <Common/assertion.h>

#include <thread>

namespace ph::editor
{

class RenderThread;
class GHIThread;

class Threads final : private IUninstantiable
{
public:
	static bool isOnMainThread();
	static bool isOnRenderThread();
	static bool isOnGHIThread();

	static RenderThread& getRenderThread();
	static GHIThread& getGHIThread();

private:
	friend class Program;
	friend class Application;

	static void setMainThreadID(std::thread::id threadID);
	static void setRenderThread(RenderThread* inRenderThread);
	static void setGHIThread(GHIThread* inGhiThread);

	static std::thread::id mainThreadID;
	static RenderThread* renderThread;
	static GHIThread* ghiThread;
};

inline bool Threads::isOnMainThread()
{
	PH_ASSERT(mainThreadID != std::thread::id());

	return std::this_thread::get_id() == mainThreadID;
}

}// end namespace ph::editor
