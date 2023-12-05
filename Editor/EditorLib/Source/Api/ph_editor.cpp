#include "ph_editor.h"
#include "imgui_main.h"
#include "EditorCore/Program.h"
#include "App/Application.h"
#include "Procedure/TestProcedureModule.h"
#include "Render/Imgui/ImguiRenderModule.h"
#include "Render/EditorDebug/EditorDebugRenderModule.h"
#include "Render/Imgui/Editor/ImguiEditorLog.h"

// Designer Scenes
#include "Designer/DesignerScene.h"

// General Designer Objects
#include "Designer/DesignerObject.h"
#include "Designer/AbstractDesignerObject.h"
#include "Designer/FlatDesignerObject.h"
#include "Designer/HierarchicalDesignerObject.h"
#include "Designer/TextualNoteObject.h"
#include "Designer/NodeObject.h"

// Rendering
#include "Designer/Render/RenderAgent.h"
#include "Designer/Render/OfflineRenderAgent.h"

// Imposters
#include "Designer/Imposter/ImposterObject.h"
#include "Designer/Imposter/GeneralImposter.h"

// Enums
#include "RenderCore/SDL/sdl_graphics_api_type.h"

#include <ph_core.h>
#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/exception.h>
#include <DataIO/FileSystem/Path.h>
#include <Utility/traits.h>

#include <cstdlib>
#include <vector>

namespace ph::editor
{

namespace
{

/*!
Important note: At this point the engine has not been initialized yet--creation of the settings must
not invoke any engine functionalities.
*/
inline EngineInitSettings get_editor_engine_init_settings()
{
	EngineInitSettings settings;
	settings.additionalLogHandlers.push_back(ImguiEditorLog::engineLogHook);
	return settings;
}

template<typename SdlClassType>
inline const SdlClass* register_editor_sdl_class()
{
	const SdlClass* const clazz = SdlClassType::getSdlClass();

	// Register for dynamic designer object creation
	if constexpr(CDerived<SdlClassType, DesignerObject>)
	{
		DesignerScene::registerObjectType<SdlClassType>();
	}

	return clazz;
}

template<typename EnumType>
inline const SdlEnum* register_editor_sdl_enum()
{
	return TSdlEnum<EnumType>::getSdlEnum();
}

inline std::vector<const SdlClass*> register_editor_classes()
{
	return
	{
		// Designer Scenes
		register_editor_sdl_class<DesignerScene>(),

		// General Designer Objects
		register_editor_sdl_class<AbstractDesignerObject>(),
		register_editor_sdl_class<DesignerObject>(),
		register_editor_sdl_class<FlatDesignerObject>(),
		register_editor_sdl_class<HierarchicalDesignerObject>(),
		register_editor_sdl_class<TextualNoteObject>(),
		register_editor_sdl_class<NodeObject>(),

		// Rendering
		register_editor_sdl_class<RenderAgent>(),
		register_editor_sdl_class<OfflineRenderAgent>(),

		// Imposters
		register_editor_sdl_class<ImposterObject>(),
		register_editor_sdl_class<GeneralImposter>(),
	};
}

inline std::vector<const SdlEnum*> register_editor_enums()
{
	return
	{
		register_editor_sdl_enum<ghi::EGraphicsAPI>(),
	};
}

}// end anonymous namespace

int application_entry_point(int argc, char* argv[])
{
	if(!init_render_engine(get_editor_engine_init_settings()))
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
	PH_DEFAULT_LOG_DEBUG("initialized {} editor SDL enum definitions", sdlEnums.size());

	// Get SDL classes once here to initialize them--this is not required,
	// same reason as SDL enums.
	//
	const auto sdlClasses = get_registered_editor_classes();
	PH_DEFAULT_LOG_DEBUG("initialized {} editor SDL class definitions", sdlClasses.size());

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

std::span<const SdlClass* const> get_registered_editor_classes()
{
	static std::vector<const SdlClass*> classes = register_editor_classes();
	return classes;
}

std::span<const SdlEnum* const> get_registered_editor_enums()
{
	static std::vector<const SdlEnum*> enums = register_editor_enums();
	return enums;
}

Path get_editor_data_directory()
{
	return Path("./EditorData/");
}

}// end namespace ph::editor
