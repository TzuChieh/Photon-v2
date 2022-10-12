#pragma once

#include "EditorCore/Thread/Threads.h"

#include <Common/assertion.h>
#include <Utility/Concurrent/TLockFreeQueue.h>
#include <Utility/TFunction.h>

#include <cstddef>
#include <vector>
#include <utility>

namespace ph::editor
{

class EditorEventQueue final
{
public:
	// Limit the number of concurrent works updated to avoid starvation on main thread works
	inline constexpr static std::size_t maxAnyThreadWorksPerUpdate = 1024;

	using EventUpdateWork = TFunction<void(void)>;

	EditorEventQueue();

	void add(EventUpdateWork work);
	void addFromAnyThread(EventUpdateWork work);
	void updateAllEvents();

private:
	void updateMainThreadEvents();
	void updateAnyThreadEvents();

	std::vector<EventUpdateWork> m_mainThreadWorks;
	TLockFreeQueue<EventUpdateWork> m_anyThreadWorks;
};

inline void EditorEventQueue::add(EventUpdateWork work)
{
	PH_ASSERT(Threads::isOnMainThread());

	m_mainThreadWorks.push_back(std::move(work));
}

inline void EditorEventQueue::addFromAnyThread(EventUpdateWork work)
{
	m_anyThreadWorks.enqueue(std::move(work));
}

}// end namespace ph::editor
