#pragma once

#include <vector>
#include <functional>

namespace ph { class SdlClass; }
namespace ph { class SdlEnum; }

namespace ph
{

/*! @brief Initialize the render engine.

Must be called before any other use of the engine.
*/
bool init_render_engine();

/*! @brief Exit the render engine.

Must be called before leaving the engine. Any other use of the engine after
this call returned is prohibited.
*/
bool exit_render_engine();

/*! @brief Get a list of registered SDL classes from the engine.
*/
std::vector<const SdlClass*> get_registered_sdl_classes();

/*! @brief Get a list of registered SDL enums from the engine.
*/
std::vector<const SdlEnum*> get_registered_sdl_enums();

}// end namespace ph
