#pragma once

#include "EngineInitSettings.h"
#include "EEngineProject.h"
#include "DataIO/FileSystem/Path.h"

#include <span>

namespace ph { class SdlClass; }
namespace ph { class SdlEnum; }
namespace ph { class IniFile; }

namespace ph
{

/*! @brief Initialize the render engine.
Must be called before any other use of the engine.
*/
bool init_render_engine(EngineInitSettings settings);

/*! @brief Exit the render engine.
Must be called before leaving the engine. Using the engine after this call 
returned is prohibited (except some cleanup operations such as destruction).
*/
bool exit_render_engine();

/*! @brief Get a list of registered SDL classes from the engine.
*/
std::span<const SdlClass* const> get_registered_engine_classes();

/*! @brief Get a list of registered SDL enums from the engine.
*/
std::span<const SdlEnum* const> get_registered_engine_enums();

Path get_config_directory(EEngineProject project);

/*! @brief Directory to resources required by the project.
*/
Path get_internal_resource_directory(EEngineProject project);

/*! @brief Directory to additional/optional resources used by the project.
*/
Path get_resource_directory(EEngineProject project);

}// end namespace ph
