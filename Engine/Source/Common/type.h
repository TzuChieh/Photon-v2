#pragma once

// Visual Studio compiler pre-definition
#if defined(_WIN32) || defined(_WIN64)
	#if defined(_WIN64)
		#define ENVIRONMENT64
	#else
		#define ENVIRONMENT32
	#endif
#endif

// GNU compiler pre-definition
#if defined(__GNUC__) && defined(__cplusplus)
	#if defined(__x86_64__) || defined(__ppc64__)
		#define ENVIRONMENT64
	#else
		#define ENVIRONMENT32
	#endif
#endif

// if the compiler is unrecognizable, assume it's in 64-bit mode
#if !(defined(ENVIRONMENT32) || defined(ENVIRONMENT64))
	#define ENVIRONMENT64
#endif

// Photon-v2 does not support 32-bit build
#ifdef ENVIRONMENT32
	#error *** Photon-v2 does not support 32-bit build, please build for 64-bit targets ***
#endif

#include <cstdint>

typedef char           int8;
typedef unsigned char  uint8;
typedef int            int32;
typedef unsigned int   uint32;
typedef int64_t        int64;
typedef uint64_t       uint64;
typedef float          float32;
typedef double         float64;

#define PHOTON_TRUE  1
#define PHOTON_FALSE 0
