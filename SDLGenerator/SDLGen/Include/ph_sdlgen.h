#pragma once

#include "EInterfaceGenerator.h"

#include <string_view>

namespace ph::sdlgen
{

void generate_sdl_interface(EInterfaceGenerator type, std::string_view outputDirectory);

}// end namespace ph::sdlgen
