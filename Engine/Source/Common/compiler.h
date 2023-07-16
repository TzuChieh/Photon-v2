#pragma once

// TODO: distinguish between versions

#if defined(__clang__)
	#define PH_COMPILER_IS_CLANG 1
#elif defined(__GNUG__)
	#define PH_COMPILER_IS_GCC 1
#elif defined(_MSC_VER)
	#define PH_COMPILER_IS_MSVC 1
#endif

#ifndef PH_COMPILER_IS_CLANG
	#define PH_COMPILER_IS_CLANG 0
#endif

#ifndef PH_COMPILER_IS_GCC
	#define PH_COMPILER_IS_GCC 0
#endif

#ifndef PH_COMPILER_IS_MSVC
	#define PH_COMPILER_IS_MSVC 0
#endif
