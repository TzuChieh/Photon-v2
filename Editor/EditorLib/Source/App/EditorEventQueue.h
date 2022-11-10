#pragma once

#include "EditorCore/Thread/Threads.h"

#include <Utility/Concurrent/TAtomicQueue.h>
#include <Utility/TFunction.h>

#include <cstddef>
#include <vector>
#include <utility>

namespace ph::editor
{

class EditorEventQueue final
{
public:
	// Safe limit of concurrent works executed to avoid starvation on main thread works
	inline constexpr static std::size_t maxAnyThreadWorksPerUpdate = 1024;

	using EventUpdateWork = TFunction<void(void)>;

	EditorEventQueue();

	/*!
	@note Thread safe.
	*/
	void add(EventUpdateWork work);

	/*! @brief Dequeue and execute added works.
	There is a limit on the max number of concurrently added (any thread) works executed,
	see `maxAnyThreadWorksPerUpdate`. This is to avoid starvation if too many works are
	continuously being added concurrently.
	@return Do not return until works are executed.
	@note Main thread only.
	*/
	void flushAllEvents();

private:
	void updateMainThreadEvents();
	void updateAnyThreadEvents();

	std::vector<EventUpdateWork> m_mainThreadWorks;
	TAtomicQueue<EventUpdateWork> m_anyThreadWorks;
};

inline void EditorEventQueue::add(EventUpdateWork work)
{
	if(Threads::isOnMainThread())
	{
		m_mainThreadWorks.push_back(std::move(work));
	}
	else
	{
		m_anyThreadWorks.enqueue(std::move(work));
	}
}

}// end namespace ph::editor
