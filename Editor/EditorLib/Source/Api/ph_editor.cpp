#include "ph_editor.h"
#include "imgui_main.h"
#include "EditorCore/Program.h"
#include "App/Application.h"

#include <cstdlib>

namespace ph::editor
{

int application_entry_point(int argc, char* argv[])
{
	Program::onProgramStart();

	Application app(AppSettings(argc, argv));
	app.run();

	Program::onProgramExit();

	return EXIT_SUCCESS;
}

int imgui_demo_entry_point(int argc, char* argv[])
{
	return imgui_main(argc, argv);
}

}// end namespace ph::editor
