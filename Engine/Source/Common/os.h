#pragma once

#if defined(_WIN32)
	#define PH_OPERATING_SYSTEM_IS_WINDOWS
#elif defined(__linux__)
	#define PH_OPERATING_SYSTEM_IS_LINUX
#else
	#define PH_OPERATING_SYSTEM_IS_WINDOWS
#endif