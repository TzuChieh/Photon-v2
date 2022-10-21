#include "EditorCore/Program.h"
#include "EditorCore/Thread/Threads.h"

#include <Common/logging.h>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(Program, EditorCore);

void Program::onProgramStart()
{
	PH_LOG(Program, "editor program start");

	Threads::setMainThreadID(std::this_thread::get_id());
}

void Program::onProgramExit()
{
	PH_LOG(Program, "editor program exit");
}

}// end namespace ph::editor
