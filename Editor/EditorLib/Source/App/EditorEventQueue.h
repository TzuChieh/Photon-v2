#pragma once

#include <Utility/Concurrent/TLockFreeQueue.h>

namespace ph::editor
{

class AppEventQueue
{
public:
	void addFromAnyThread();

private:

};

}// end namespace ph::editor
