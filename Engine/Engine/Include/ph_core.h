#pragma once

#include "EngineInitSettings.h"
#include "EEngineProject.h"

#include <span>
#include <optional>

namespace ph { class SdlClass; }
namespace ph { class SdlEnum; }
namespace ph { class Path; }

namespace ph
{

/*! @brief Initialize the render engine.

Must be called before any other use of the engine. This functions also
initializes all dependencies. Note that initialization is generally not
required when using APIs from a base library (e.g., the `Common` library)--in
case of an exception (such as logging), consult the API's documentation to see
if anything should be done before using it.

@param settings Settings for the engine. If not provided, the engine will try to load from a
standard location (automatically create a default one if not exist).
*/
bool init_render_engine(std::optional<EngineInitSettings> settings = std::nullopt);

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

/*! @brief Directory to scripts for the project.
*/
Path get_script_directory(EEngineProject project);

}// end namespace ph
