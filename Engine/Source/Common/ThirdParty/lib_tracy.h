#pragma once

#if PH_THIRD_PARTY_HAS_TRACY

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// This is generally done by the build script. Just a failsafe here.
#ifndef TRACY_ENABLE
#define TRACY_ENABLE 1
#endif

#include <tracy/Tracy.hpp>

#endif
