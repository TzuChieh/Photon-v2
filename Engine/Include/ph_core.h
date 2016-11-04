#pragma once

// HACK
#define PH_EXPORT_API

// Checking the compiling environment.
#if defined(_MSC_VER)
/**************************************************** Microsoft Visual Studio */

	// Note: The use of dllexport implies a definition, while dllimport implies a declaration. To force a declaration,
	// an "extern" keyword with dllexport is needed; otherwise, a definition is implied.

	#ifdef PH_EXPORT_API
		#define PH_API __declspec(dllexport)
	#else
		#define PH_API __declspec(dllimport)
	#endif

#else
/************************************************************ other compilers */

	// TODO
	#ifdef EXPORT_PHOTON_API
		#define PH_API
	#else
		#define PH_API
	#endif

#endif
// end compiling environment check

// primitive data types

#include <stdint.h>

typedef char           PHint8;
typedef unsigned char  PHuint8;
typedef int            PHint32;
typedef unsigned int   PHuint32;
typedef int64_t        PHint64;
typedef uint64_t       PHuint64;
typedef float          PHfloat32;
typedef double         PHfloat64;

#define PH_TRUE  1
#define PH_FALSE 0

#ifdef __cplusplus
extern "C" {
#endif

extern PH_API PHint32 phStart();
extern PH_API void    phExit();

#ifdef __cplusplus
}
#endif