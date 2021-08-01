#pragma once

#include <string_view>

namespace ph::sdlgen
{

/*! @brief Make a standard SDL name to canonical capitalized form.

SDL names are commonly being lower-case and separated by dashes. The helper
will make the SDL name capitalized and separated by spaces.
*/
std::string sdl_name_to_capitalized(std::string_view sdlName);

}// end namespace ph::sdlgen
