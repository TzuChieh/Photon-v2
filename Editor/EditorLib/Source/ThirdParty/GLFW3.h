#pragma once

#include "ThirdParty/GLFW3_fwd.h"

// Makes the GLFW header not include any OpenGL or OpenGL ES API header, so including Glad headers
// will not result in conflicts.
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
