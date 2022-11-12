#pragma once

#include "ThirdParty/GLFW3_fwd.h"

#if PH_THIRD_PARTY_HAS_GLFW

// Makes the GLFW header not include any OpenGL or OpenGL ES API header, so including Glad headers
// will not result in conflicts.
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

#endif
