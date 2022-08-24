#pragma once

/*! @file

@brief Operating system detection macros and utilities.

The following macros will be defined for each operating system:

* Windows: `PH_OPERATING_SYSTEM_IS_WINDOWS`
* Linux: `PH_OPERATING_SYSTEM_IS_LINUX`
* macOS: `PH_OPERATING_SYSTEM_IS_OSX`

*/

// Defined on Windows x64 & x86
#if defined(_WIN32)

	#define PH_OPERATING_SYSTEM_IS_WINDOWS

// Defined on Linux
#elif defined(__linux__)

	#define PH_OPERATING_SYSTEM_IS_LINUX

// Defined on Apple platforms
// Reference: https://stackoverflow.com/questions/12132933/preprocessor-macro-for-os-x-targets
#elif defined(__APPLE__) || defined(__MACH__)

	#include <TargetConditionals.h>

	#if TARGET_OS_MAC == 1 && TARGET_OS_OSX == 1
		#define PH_OPERATING_SYSTEM_IS_OSX
	#else
		#error "Unsupported Apple operating system."
	#endif

#else

	#error "Unsupported operating system."

#endif

namespace ph::os
{

enum class EWindowsVersion
{
	UNKNOWN = 0,

	// Later version must have larger value

	Windows_2000,
	Windows_XP,
	Windows_Vista,
	Windows_7,
	Windows_8,
	Windows_8_1,
	Windows_10
};

EWindowsVersion get_windows_version();

}// end namespace ph::os
