#include "EditorCore/Program.h"
#include "EditorCore/Thread/Threads.h"

#include <Common/logging.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(Program, EditorCore);

void Program::programStart()
{
	Threads::setMainThreadID(std::this_thread::get_id());

	PH_LOG(Program, "editor program start");
}

void Program::programExit()
{
	PH_LOG(Program, "editor program exit");

	Threads::setMainThreadID(std::thread::id());
}

}// end namespace ph::editor
