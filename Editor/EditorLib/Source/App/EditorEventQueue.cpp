#include "App/EditorEventQueue.h"

#include <Common/logging.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(EditorEventQueue, Editor);

EditorEventQueue::EditorEventQueue()
	: m_mainThreadWorks()
	, m_anyThreadWorks(maxAnyThreadWorksPerUpdate)
{
	m_mainThreadWorks.reserve(128);
}

void EditorEventQueue::flushAllEvents()
{
	PH_ASSERT(Threads::isOnMainThread());

	updateMainThreadEvents();
	updateAnyThreadEvents();
}

void EditorEventQueue::updateMainThreadEvents()
{
	PH_ASSERT(Threads::isOnMainThread());

	for(const EventUpdateWork& work : m_mainThreadWorks)
	{
		work();
	}

	m_mainThreadWorks.clear();
}

void EditorEventQueue::updateAnyThreadEvents()
{
	PH_ASSERT(Threads::isOnMainThread());

	std::size_t numDequeuedWorks = 0;
	EventUpdateWork work;
	while(m_anyThreadWorks.tryDequeue(&work))
	{
		++numDequeuedWorks;

		work();

		if(numDequeuedWorks == maxAnyThreadWorksPerUpdate + 1)
		{
			PH_LOG_WARNING(EditorEventQueue,
				"too many concurrently added editor events ({}), current safe limit is {}",
				numDequeuedWorks + m_anyThreadWorks.estimatedSize(), maxAnyThreadWorksPerUpdate);
		}
	}
}

}// end namespace ph::editor
