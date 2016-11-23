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

#define PH_BRUTE_FORCE_RENDERER_TYPE 1
#define PH_IMPORTANCE_RENDERER_TYPE  2

#define PH_HDR_FRAME_TYPE 50

#define PH_DEFAULT_CAMERA_TYPE 100

#define PH_STANDARD_SAMPLE_GENERATOR_TYPE 150

#ifdef __cplusplus
extern "C" {
#endif

extern PH_API PHint32 phStart();
extern PH_API void    phExit();

extern PH_API void phCreateRenderer(PHuint64* out_rendererId, const PHint32 rendererType);
extern PH_API void phDeleteRenderer(const PHuint64 rendererId);

extern PH_API void phCreateWorld(PHuint64* out_worldId);
extern PH_API void phDeleteWorld(const PHuint64 worldId);

extern PH_API void phCreateFrame(PHuint64* out_frameId, const PHint32 frameType, const PHuint32 frameWidthPx, const PHuint32 frameHeightPx);
extern PH_API void phDeleteFrame(const PHuint64 frameId);

extern PH_API void phCreateCamera(PHuint64* out_cameraId, const PHint32 cameraType);
extern PH_API void phDeleteCamera(const PHuint64 cameraId);

extern PH_API void phCreateSampleGenerator(PHuint64* out_sampleGeneratorId, const PHint32 sampleGeneratorType, const PHuint32 sppBudget);
extern PH_API void phDeleteSampleGenerator(const PHuint64 sampleGeneratorId);

#ifdef __cplusplus
}
#endif