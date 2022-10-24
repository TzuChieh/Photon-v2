#include "EditorCore/Thread/Threads.h"
#include "EditorCore/Thread/RenderThread.h"
#include "EditorCore/Thread/GHIThread.h"

namespace ph::editor
{

std::thread::id Threads::mainThreadID;
RenderThread* Threads::renderThread = nullptr;
GHIThread* Threads::ghiThread = nullptr;

void Threads::setMainThreadID(const std::thread::id threadID)
{
	PH_ASSERT(threadID != std::thread::id());
	PH_ASSERT(mainThreadID == std::thread::id());

	mainThreadID = threadID;
}

void Threads::setRenderThread(RenderThread* const inRenderThread)
{
	PH_ASSERT(inRenderThread);
	PH_ASSERT(!renderThread);

	renderThread = inRenderThread;
}

void Threads::setGHIThread(GHIThread* const inGhiThread)
{
	PH_ASSERT(inGhiThread);
	PH_ASSERT(!ghiThread);

	ghiThread = inGhiThread;
}

bool Threads::isOnRenderThread()
{
	return std::this_thread::get_id() == getRenderThread().getWorkerThreadId();
}

bool Threads::isOnGHIThread()
{
	return std::this_thread::get_id() == getGHIThread().getWorkerThreadId();
}

RenderThread& Threads::getRenderThread()
{
	PH_ASSERT(renderThread);

	return *renderThread;
}

GHIThread& Threads::getGHIThread()
{
	PH_ASSERT(ghiThread);

	return *ghiThread;
}

}// end namespace ph::editor
