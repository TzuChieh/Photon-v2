#pragma once

// TODO: distinguish between versions

#if defined(__clang__)
	#define PH_COMPILER_IS_CLANG
#elif defined(__GNUG__)
	#define PH_COMPILER_IS_GCC
#elif defined(_MSC_VER)
	#define PH_COMPILER_IS_MSVC
#endif
