#include "ph_editor.h"
#include "imgui_main.h"
#include "EditorCore/Program.h"
#include "App/Application.h"
#include "Procedure/TestProcedureModule.h"
#include "Render/Imgui/ImguiRenderModule.h"
#include "Render/EditorDebug/EditorDebugRenderModule.h"

#include <Common/assertion.h>
#include <Utility/exception.h>

#include <cstdlib>

namespace ph::editor
{

int application_entry_point(int argc, char* argv[])
{
	Program::onProgramStart();

	// App should not outlive program
	try
	{
		Application app(AppSettings(argc, argv));

		TestProcedureModule testModule;
		ImguiRenderModule imguiModule;
		EditorDebugRenderModule editorDebugRenderModule;
		app.attachProcedureModule(&testModule);
		app.attachRenderModule(&imguiModule);
		app.attachRenderModule(&editorDebugRenderModule);

		app.run();

		app.detachProcedureModule(&testModule);
		app.detachRenderModule(&imguiModule);
		app.detachRenderModule(&editorDebugRenderModule);

		app.close();
	}
	catch(const Exception& e)
	{
		PH_DEFAULT_LOG_ERROR("unhandled exception thrown: {}",
			e.what());

		PH_DEBUG_BREAK();
	}

	Program::onProgramExit();

	return EXIT_SUCCESS;
}

int imgui_demo_entry_point(int argc, char* argv[])
{
	return imgui_main(argc, argv);
}

}// end namespace ph::editor
