#pragma once

/*! @file

@brief Compiler-related information and utilities.

## Definitions for Detecting Compilers

- `PH_COMPILER_IS_MSVC`: 1 if the compiler is the Microsoft Visual C++ compiler (MSVC), 0 otherwise
- `PH_COMPILER_IS_GNU`: 1 if the compiler is the GNU C++ compiler, 0 otherwise
- `PH_COMPILER_IS_CLANG`: 1 if the compiler is Clang C++ compiler, 0 otherwise

## Additional Attributes

Introduces additional attributes for use with the standard C++ attribute syntax.

- `[[PH_ALWAYS_INLINE]]`: Always attempt to inline the target
*/

/* Compiler detection. */

#if defined(__clang__)
	#define PH_COMPILER_IS_CLANG 1
#elif defined(__GNUG__)
	#define PH_COMPILER_IS_GNU 1
#elif defined(_MSC_VER)
	#define PH_COMPILER_IS_MSVC 1
#endif

#ifndef PH_COMPILER_IS_CLANG
	#define PH_COMPILER_IS_CLANG 0
#endif

#ifndef PH_COMPILER_IS_GNU
	#define PH_COMPILER_IS_GNU 0
#endif

#ifndef PH_COMPILER_IS_MSVC
	#define PH_COMPILER_IS_MSVC 0
#endif

/* Attribute: Always attempt to inline the target. */

#if PH_COMPILER_IS_MSVC
	#define PH_ALWAYS_INLINE msvc::forceinline
#elif PH_COMPILER_IS_GNU
	#define PH_ALWAYS_INLINE gnu::always_inline
#elif PH_COMPILER_IS_CLANG
	#define PH_ALWAYS_INLINE clang::always_inline
#else
	#error "Unrecognized compiler: cannot define `PH_ALWAYS_INLINE`"
#endif

/* Feature detection for P2468R2: "The Equality Operator You Are Looking For",
see https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2468r2.html.
*/

#if PH_COMPILER_IS_MSVC
	// As of Visual Studio 17.10.1, this is not supported yet.
	#define PH_COMPILER_HAS_P2468R2 0
#elif PH_COMPILER_IS_GNU
	#define PH_COMPILER_HAS_P2468R2 (__GNUC__ >= 13)
#elif PH_COMPILER_IS_CLANG
	#define PH_COMPILER_HAS_P2468R2 (__clang_major__ >= 16)
#else
	#define PH_COMPILER_HAS_P2468R2 0
#endif
