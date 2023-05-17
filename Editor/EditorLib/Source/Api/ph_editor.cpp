#include "ph_editor.h"
#include "imgui_main.h"
#include "EditorCore/Program.h"
#include "App/Application.h"
#include "Procedure/TestProcedureModule.h"
#include "Render/Imgui/ImguiRenderModule.h"
#include "Render/EditorDebug/EditorDebugRenderModule.h"
#include "Designer/AbstractDesignerObject.h"
#include "Designer/DesignerObject.h"
#include "Designer/FlatDesignerObject.h"
#include "Designer/HierarchicalDesignerObject.h"

#include <ph_cpp_core.h>
#include <Common/assertion.h>
#include <Utility/exception.h>

#include <cstdlib>
#include <iostream>

namespace ph::editor
{

int application_entry_point(int argc, char* argv[])
{
	if(!init_render_engine(EngineInitSettings()))
	{
		return EXIT_FAILURE;
	}

	Program::programStart();

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

	Program::programExit();

	if(!exit_render_engine())
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int imgui_demo_entry_point(int argc, char* argv[])
{
	return imgui_main(argc, argv);
}

namespace
{

template<typename SdlClassType>
const SdlClass* get_sdl_class()
{
	return SdlClassType::getSdlClass();
}

}// end anonymous namespace

std::vector<const SdlClass*> get_registered_editor_classes()
{
	return
	{
		get_sdl_class<AbstractDesignerObject>(),
		get_sdl_class<DesignerObject>(),
		get_sdl_class<FlatDesignerObject>(),
		get_sdl_class<HierarchicalDesignerObject>(),
	};
}

}// end namespace ph::editor
