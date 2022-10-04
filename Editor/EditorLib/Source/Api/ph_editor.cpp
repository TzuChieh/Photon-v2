#include "ph_editor.h"
#include "imgui_main.h"
#include "EditorCore/Program.h"

#include <cstdlib>

namespace ph::editor
{

int application_entry_point(int argc, char* argv[])
{
	Program::onProgramStart();

	// TODO

	Program::onProgramExit();

	return EXIT_SUCCESS;
}

int imgui_demo_entry_point(int argc, char* argv[])
{
	return imgui_main(argc, argv);
}

}// end namespace ph::editor
