#include "EditorLib/ph_editor.h"
#include "EditorLib/imgui_main.h"
#include "EditorLib/EditorCore/Program.h"
#include "EditorLib/App/Application.h"
#include "EditorLib/Procedure/TestProcedureModule.h"
#include "EditorLib/Render/Imgui/ImguiRenderModule.h"
#include "EditorLib/Render/EditorDebug/EditorDebugRenderModule.h"
#include "EditorLib/Render/Imgui/Editor/ImguiEditorLog.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/exceptions.h>
#include <Engine/ph_core.h>
#include <Engine/DataIO/FileSystem/Path.h>
#include <Engine/DataIO/FileSystem/Filesystem.h>
#include <Engine/Utility/traits.h>
#include <Engine/SDL/sdl_meta.h>

#include <cstdlib>
#include <vector>

namespace ph::editor
{

PH_DECLARE_GETTER_FOR_ALL_SDL_CLASSES(gather_all_editor_SDL_classes, outerScope=editor);
PH_DECLARE_GETTER_FOR_ALL_SDL_ENUMS(gather_all_editor_SDL_enums, outerScope=editor);

int application_entry_point(int argc, char* argv[])
{
	// At this point the engine has not been initialized yet; creation of the settings must
	// not invoke any engine functionality.
	auto engineInitSettings = EngineInitSettings::loadStandardConfig();
	engineInitSettings.additionalLogHandlers.push_back(ImguiEditorLog::engineLogHook);

	if(!init_render_engine(engineInitSettings))
	{
		return EXIT_FAILURE;
	}

	// Get SDL enums once here to initialize them--this is not required, just to be safe 
	// as SDL enum instances are lazy-constructed and may be done in strange places/order 
	// later (which may cause problems). Also, there may be some extra code in the definition
	// that want to be ran early.
	// Enums are initialized first as they have fewer dependencies.
	//
	const auto sdlEnums = get_registered_editor_enums();
	PH_DEFAULT_DEBUG_LOG("initialized {} editor SDL enum definitions", sdlEnums.size());

	// Get SDL classes once here to initialize them--this is not required,
	// same reason as SDL enums.
	//
	const auto sdlClasses = get_registered_editor_classes();
	PH_DEFAULT_DEBUG_LOG("initialized {} editor SDL class definitions", sdlClasses.size());

	Program::programStart();

	// App should not outlive program (specifically, within `Program` start & exit)
	try
	{
		Application app(argc, argv);

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
		PH_DEFAULT_LOG(Error, "unhandled exception thrown: {}",
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

std::span<const SdlClass* const> get_registered_editor_classes()
{
	/* The following section registeres SDL classes and enums to the editor. Please note that SDL
	interface definition and reflection do not need registration to work, this simply provide an
	interface to available classes and enums so some functionalities can benefit from it.
	*/
	static std::vector<const SdlClass*> classes = gather_all_editor_SDL_classes();
	return classes;
}

std::span<const SdlEnum* const> get_registered_editor_enums()
{
	/* The following section registeres SDL classes and enums to the editor. Please note that SDL
	interface definition and reflection do not need registration to work, this simply provide an
	interface to available classes and enums so some functionalities can benefit from it.
	*/
	static std::vector<const SdlEnum*> enums = gather_all_editor_SDL_enums();
	return enums;
}

Path get_editor_data_directory()
{
	return Filesystem::getInstallationDirectory() / "EditorData";
}

}// end namespace ph::editor
