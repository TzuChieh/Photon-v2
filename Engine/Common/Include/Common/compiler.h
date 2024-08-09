#pragma once

/*! @file

@brief Compiler-related information and utilities.

## Definitions for Detecting Compilers

- `PH_COMPILER_IS_MSVC`: 1 if the compiler is the Microsoft Visual C++ compiler (MSVC), 0 otherwise
- `PH_COMPILER_IS_GCC`: 1 if the compiler is the GNU C++ compiler, 0 otherwise
- `PH_COMPILER_IS_CLANG`: 1 if the compiler is Clang C++ compiler, 0 otherwise

## Additional Attributes

Introduces additional attributes for use with the standard C++ attribute syntax.

- `[[PH_ALWAYS_INLINE]]`: Always attempt to inline the target
*/

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

#if PH_COMPILER_IS_MSVC
	#define PH_ALWAYS_INLINE msvc::forceinline
#elif PH_COMPILER_IS_GCC
	#define PH_ALWAYS_INLINE gnu::always_inline
#elif PH_COMPILER_IS_CLANG
	#define PH_ALWAYS_INLINE clang::always_inline
#else
	#error "Unrecognized compiler: cannot define `PH_ALWAYS_INLINE`"
#endif
