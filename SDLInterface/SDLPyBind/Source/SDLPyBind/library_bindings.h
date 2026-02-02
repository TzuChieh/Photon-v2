#pragma once

/*! @file library_bindings.h
  
@brief Python binding generation utilities.

To add new bindings, basically you will need to do the following steps:
1. Create SDL definition for the C++ code and specify "interface=python".
2. Make sure the SDL definition is accessible to this project (public headers, linking, etc.).
3. Run setup script and regenerate the project through CMake.
4. Bind it here.

*/

#include <nanobind/nanobind.h>

namespace ph::py
{

void bind_engine_sdl_definitions(nanobind::module_& m);

}// end namespace ph::py
