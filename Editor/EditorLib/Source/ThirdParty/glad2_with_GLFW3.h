#pragma once

#if PH_THIRD_PARTY_HAS_GLAD
#include <glad/gl.h>
#endif

// Make sure GLFW includes after glad to avoid potential conflicts (see "ThirdParty/GLFW3_fwd.h"
// for more details).
#include "ThirdParty/GLFW3.h"
