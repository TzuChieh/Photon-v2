#pragma once

#if defined(_WIN32)
	#define PH_OPERATING_SYSTEM_IS_WINDOWS
#elif defined(__linux__)
	#define PH_OPERATING_SYSTEM_IS_LINUX
#else
	#define PH_OPERATING_SYSTEM_IS_OSX
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
