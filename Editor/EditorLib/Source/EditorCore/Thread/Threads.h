#pragma once

#include <Utility/IUninstantiable.h>
#include <Common/assertion.h>

#include <thread>

namespace ph::editor
{

class Threads final : private IUninstantiable
{
public:
	static bool isOnMainThread();
	static bool isOnRenderThread();

private:
	friend class Program;

	static void setMainThreadID(std::thread::id threadID);
	static void setRenderThreadID(std::thread::id threadID);

	static std::thread::id mainThreadID;
	static std::thread::id renderThreadID;
};

inline bool Threads::isOnMainThread()
{
	PH_ASSERT(mainThreadID != std::thread::id());
	return std::this_thread::get_id() == mainThreadID;
}

inline bool Threads::isOnRenderThread()
{
	PH_ASSERT(renderThreadID != std::thread::id());
	return std::this_thread::get_id() == renderThreadID;
}

}// end namespace ph::editor
