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

#define PH_HDR_FRAME_TYPE 50

#ifdef __cplusplus
extern "C" {
#endif

extern PH_API void phCreateEngine(PHuint64* out_engineId, const PHuint32 numRenderThreads);
extern PH_API void phEnterCommand(const PHuint64 engineId, const char* const commandFragment);
extern PH_API void phRender(const PHuint64 engineId);
extern PH_API void phDevelopFilm(const PHuint64 engineId, const PHuint64 frameId);
extern PH_API void phDeleteEngine(const PHuint64 engineId);

// TODO: frame related API needs to aware of the size of real

extern PH_API void phCreateFrame(PHuint64* out_frameId, const PHint32 frameType);
extern PH_API void phGetFrameData(const PHuint64 frameId, const PHfloat32** out_data, PHuint32* out_widthPx, PHuint32* out_heightPx, PHuint32* out_nPixelComponents);
extern PH_API void phDeleteFrame(const PHuint64 frameId);

#ifdef __cplusplus
}
#endif