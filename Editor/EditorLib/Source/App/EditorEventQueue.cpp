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

void EditorEventQueue::updateAllEvents()
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
	while(numDequeuedWorks < maxAnyThreadWorksPerUpdate && m_anyThreadWorks.tryDequeue(&work))
	{
		++numDequeuedWorks;

		work();
	}

	if(numDequeuedWorks == maxAnyThreadWorksPerUpdate && m_anyThreadWorks.estimatedSize() > 0)
	{
		PH_LOG_WARNING(EditorEventQueue,
			"too many concorrently added editor events, {} are delayed to next frame", 
			m_anyThreadWorks.estimatedSize());
	}
}

}// end namespace ph::editor
