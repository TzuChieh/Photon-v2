#include "EditorCore/Thread/Threads.h"
#include "EditorCore/Thread/RenderThread.h"

#include <Utility/exception.h>

namespace ph::editor
{

std::thread::id Threads::mainThreadID;
std::thread::id Threads::renderThreadID;

void Threads::setMainThreadID(const std::thread::id threadID)
{
	// Never reset existing valid thread ID to another valid one
	// (switching main thread should not happen)
	if(threadID != std::thread::id() && mainThreadID != std::thread::id())
	{
		throw LogicalException(
			"Attempting to switch main thread, this is an illegal operation.");
	}

	mainThreadID = threadID;
}

void Threads::setRenderThreadID(const std::thread::id threadID)
{
	// Never reset existing valid thread ID to another valid one
	// (switching render thread should not happen)
	if(threadID != std::thread::id() && renderThreadID != std::thread::id())
	{
		throw LogicalException(
			"Attempting to switch render thread, this is an illegal operation.");
	}

	renderThreadID = threadID;
}

}// end namespace ph::editor
