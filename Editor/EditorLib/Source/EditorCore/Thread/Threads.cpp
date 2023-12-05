#include "EditorCore/Thread/Threads.h"
#include "Render/RenderThread.h"

#include <Common/exception.h>

namespace ph::editor
{

std::thread::id Threads::mainThreadID;
std::thread::id Threads::renderThreadID;
RenderThread* Threads::renderThread = nullptr;

void Threads::setMainThreadID(const std::thread::id threadID)
{
	// Never reset existing valid thread ID to another valid one
	// (switching main thread should not happen)
	if(threadID != std::thread::id{} && mainThreadID != std::thread::id{})
	{
		throw LogicalException(
			"Attempting to switch main thread, this is an illegal operation.");
	}

	mainThreadID = threadID;
}

void Threads::setRenderThread(RenderThread* const thread)
{
	// Switching render thread should not happen
	if(thread && renderThread)
	{
		throw LogicalException(
			"Attempting to switch render thread, this is an illegal operation.");
	}

	renderThreadID = thread ? thread->getWorkerThreadId() : std::thread::id{};
	renderThread = thread;
}

}// end namespace ph::editor
