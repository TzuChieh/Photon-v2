#include "EditorCore/Program.h"
#include "EditorCore/Thread/Threads.h"

#include <Common/logging.h>

#include <chrono>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(Program, EditorCore);

uint64 Program::startTime = 0;

uint64 Program::getStartTime()
{
	return startTime;
}

void Program::programStart()
{
	auto absTime = std::chrono::steady_clock::now().time_since_epoch();
	startTime = static_cast<uint64>(std::chrono::nanoseconds(absTime).count());

	Threads::setMainThreadID(std::this_thread::get_id());

	PH_LOG(Program, Note, "editor program start");
}

void Program::programExit()
{
	PH_LOG(Program, Note, "editor program exit");

	Threads::setMainThreadID(std::thread::id());
}

}// end namespace ph::editor
