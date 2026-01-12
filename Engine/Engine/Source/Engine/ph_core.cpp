#include "Engine/ph_core.h"
#include "Engine/init_and_exit.h"
#include "Engine/DataIO/FileSystem/Path.h"
#include "Engine/DataIO/FileSystem/Filesystem.h"
#include "Engine/SDL/sdl_meta.h"

#include <Common/config.h>
#include <Common/logging.h>
#include <Common/Log/Logger.h>

#include <utility>
#include <vector>
#include <string>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(CppAPI, Engine);

PH_DECLARE_GETTER_FOR_ALL_SDL_CLASSES(gather_all_engine_SDL_classes, outerScope=void);
PH_DECLARE_GETTER_FOR_ALL_SDL_ENUMS(gather_all_engine_SDL_enums, outerScope=void);

bool init_render_engine(std::optional<EngineInitSettings> settings)
{
	detail::core_logging::init();

	if(!settings)
	{
		settings = EngineInitSettings::loadStandardConfig();
	}

	if(!settings->additionalLogHandlers.empty())
	{
		PH_LOG(CppAPI, Note, "adding {} additional log handler(s)", settings->additionalLogHandlers.size());
		for(LogHandler& handler : settings->additionalLogHandlers)
		{
			if(!handler)
			{
				PH_LOG(CppAPI, Warning,
					"attempting to add a null core log handler");
				continue;
			}
			
			detail::core_logging::get_logger().addLogHandler(std::move(handler));
		}

		settings->additionalLogHandlers.clear();
	}

	if(!init_engine_core(*settings))
	{
		PH_LOG(CppAPI, Error, "core initialization failed");
		return false;
	}

	if(!init_engine_IO_infrastructure(*settings))
	{
		PH_LOG(CppAPI, Error, "IO infrastructure initialization failed");
		return false;
	}

	// Get SDL enums once here to initialize them--this is not required, just to be safe 
	// as SDL enum instances are lazy-constructed and may be done in strange places/order 
	// later (which may cause problems). Also, there may be some extra code in the definition
	// that want to be ran early.
	// Enums are initialized first as they have fewer dependencies.
	//
	const auto sdlEnums = get_registered_engine_enums();
	PH_DEBUG_LOG(CppAPI, "initialized {} SDL enum definitions", sdlEnums.size());

	// Get SDL classes once here to initialize them--this is not required,
	// same reason as SDL enums.
	//
	const auto sdlClasses = get_registered_engine_classes();
	PH_DEBUG_LOG(CppAPI, "initialized {} SDL class definitions", sdlClasses.size());

	after_engine_init(*settings);

	return true;
}

bool exit_render_engine()
{
	before_engine_exit();

	if(!exit_API_database())
	{
		PH_LOG(CppAPI, Error, "C API database exiting failed");
		return false;
	}

	detail::core_logging::exit();

	return true;
}

std::span<const SdlClass* const> get_registered_engine_classes()
{
	/* Registeres SDL classes and enums to the engine. Please note that SDL
	interface definition and reflection system do not need registration to work, this simply provide an
	interface to available classes and enums so some functionalities can benefit from it.
	*/
	static std::vector<const SdlClass*> classes = gather_all_engine_SDL_classes();
	return classes;
}

std::span<const SdlEnum* const> get_registered_engine_enums()
{
	/* Registeres SDL classes and enums to the engine. Please note that SDL
	interface definition and reflection system do not need registration to work, this simply provide an
	interface to available classes and enums so some functionalities can benefit from it.
	*/
	static std::vector<const SdlEnum*> enums = gather_all_engine_SDL_enums();
	return enums;
}

Path get_config_directory(const EEngineProject project)
{
	return Filesystem::getConfigDirectory() / to_string(project);
}

Path get_internal_resource_directory(const EEngineProject project)
{
	return Filesystem::getInternalResourceDirectory() / to_string(project);
}

Path get_resource_directory(const EEngineProject project)
{
	return Filesystem::getResourceDirectory() / to_string(project);
}

Path get_intermediate_directory(EEngineProject project)
{
	return Filesystem::getIntermediateDirectory() / to_string(project);
}

Path get_script_directory(const EEngineProject project)
{
	return Filesystem::getScriptDirectory() / to_string(project);
}

}// end namespace ph
