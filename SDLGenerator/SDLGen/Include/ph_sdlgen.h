#pragma once

#include "EInterfaceGenerator.h"

#include <string_view>

namespace ph::sdlgen
{

/*! @brief Make a standard SDL name to canonical capitalized form.

SDL names are commonly being lower-case and separated by dashes. The helper
will make the SDL name capitalized and separated by spaces.
*/
std::string sdl_name_to_capitalized(std::string_view sdlName);

void generate_sdl_interface(EInterfaceGenerator type, std::string_view outputDirectory);

}// end namespace ph::sdlgen
