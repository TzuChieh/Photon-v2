#pragma once

#if PH_THIRD_PARTY_HAS_TRACY

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// This should be done by the build script.
#ifndef TRACY_ENABLE
#error "Tracy profiler is not enabled. Please make sure `TRACY_ENABLE` is defined globally by the build script."
#endif

#include <tracy/Tracy.hpp>

#endif
