#include "ph_editor.h"
#include "imgui_main.h"
#include "EditorCore/Program.h"
#include "App/Application.h"
#include "Procedure/TestProcedureModule.h"
#include "Render/Imgui/ImguiRenderModule.h"

#include <cstdlib>

namespace ph::editor
{

int application_entry_point(int argc, char* argv[])
{
	Program::onProgramStart();

	// App should not outlive program
	{
		Application app(AppSettings(argc, argv));

		TestProcedureModule testModule;
		ImguiRenderModule imguiModule;
		app.attachProcedureModule(&testModule);
		app.attachRenderModule(&imguiModule);

		app.run();

		app.detachProcedureModule(&testModule);
		app.detachRenderModule(&imguiModule);

		app.close();
	}

	Program::onProgramExit();

	return EXIT_SUCCESS;
}

int imgui_demo_entry_point(int argc, char* argv[])
{
	return imgui_main(argc, argv);
}

}// end namespace ph::editor
