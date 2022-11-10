#include "App/Editor.h"
#include "EditorCore/Thread/Threads.h"

#include <Common/assertion.h>

namespace ph::editor
{

void Editor::flushAllEvents()
{
	PH_ASSERT(Threads::isOnMainThread());

	// Post events to the event process queue
	m_eventPostQueue.flushAllEvents();

	// Run all event process work
	for(const auto& processEventWork : m_eventProcessQueue)
	{
		processEventWork();
	}

	m_eventProcessQueue.clear();
}

}// end namespace ph::editor
