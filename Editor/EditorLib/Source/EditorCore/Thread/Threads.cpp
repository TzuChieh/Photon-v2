#include "EditorCore/Thread/Threads.h"

namespace ph::editor
{

std::thread::id Threads::mainThreadID;
std::thread::id Threads::renderThreadID;

void Threads::setMainThreadID(const std::thread::id threadID)
{
	PH_ASSERT(threadID != std::thread::id());
	mainThreadID = threadID;
}

void Threads::setRenderThreadID(const std::thread::id threadID)
{
	PH_ASSERT(threadID != std::thread::id());
	renderThreadID = threadID;
}

}// end namespace ph::editor
