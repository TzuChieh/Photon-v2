#pragma once

#include <Utility/Concurrent/TLockFreeQueue.h>

namespace ph::editor
{

class AppEventQueue
{
public:
	void addFromAnyThread();
	void add();

	// TODO: max. allowed concurrent event addition (to avoid starvation on main thread)

private:

};

}// end namespace ph::editor
