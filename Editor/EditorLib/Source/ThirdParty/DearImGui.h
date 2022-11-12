#pragma once

#include <imgui.h>

// TODO: in the future we may need to separately include these impl files or provide our own version 

#if PH_THIRD_PARTY_HAS_GLFW
#include <imgui_impl_glfw.h>
#endif

#include <imgui_impl_opengl3.h>
