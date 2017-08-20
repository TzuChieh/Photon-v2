#pragma once

/////////////////////////////////////////////////////////////////////
// 
// To correctly use Photon-v2 API, please read the following notes:
//
// 1. phInit() must be called before any other use of the API.
// 
// 2. phExit() must be called after finishing the use of the API.
// 
// 3. phInit() and phExit() must be called on the same thread.
//
// 4. phCreate<X>() and phAsync<X>() functions can be used in a
//    multithreaded environment, namely, they are thread-safe.
// 
// 5. Resources created by phCreate<X>() cannot be manipulated 
//    concurrently. Any function requiring some resource ID inputs 
//    (except phAsync<X>() functions) is considered a resource 
//    manipulating operation on those resources.
// 
/////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// starting and exiting Photon
//

extern PH_API int phInit();
extern PH_API int phExit();

///////////////////////////////////////////////////////////////////////////////
// core engine operations
//

extern PH_API void phCreateEngine(PHuint64* out_engineId, const PHuint32 numRenderThreads);
extern PH_API void phEnterCommand(PHuint64 engineId, const char* commandFragment);
extern PH_API void phRender(PHuint64 engineId);
extern PH_API void phUpdate(PHuint64 engineId);
extern PH_API void phDevelopFilm(PHuint64 engineId, PHuint64 frameId);
extern PH_API void phGetFilmDimension(PHuint64 engineId, PHuint32* out_widthPx, PHuint32* out_heightPx);
extern PH_API void phDeleteEngine(PHuint64 engineId);


///////////////////////////////////////////////////////////////////////////////
// frame operations
//

extern PH_API void phCreateFrame(PHuint64* out_frameId, PHuint32 widthPx, PHuint32 heightPx);
extern PH_API void phGetFrameDimension(PHuint64 frameId, PHuint32* out_widthPx, PHuint32* out_heightPx);
extern PH_API void phGetFrameRgbData(PHuint64 frameId, const PHfloat32** out_data);
extern PH_API void phDeleteFrame(PHuint64 frameId);

///////////////////////////////////////////////////////////////////////////////
// asynchronous operations
//

#define PH_FILM_REGION_STATUS_INVALID  -1
#define PH_FILM_REGION_STATUS_UPDATING 1
#define PH_FILM_REGION_STATUS_FINISHED 2

extern PH_API void phAsyncGetRendererStatistics(PHuint64 engineId, 
                                                PHfloat32* out_percentageProgress, 
                                                PHfloat32* out_samplesPerSecond);

extern PH_API int  phAsyncPollUpdatedFilmRegion(PHuint64 engineId,
                                                PHuint32* out_xPx, PHuint32* out_yPx, 
                                                PHuint32* out_widthPx, PHuint32* out_heightPx);

extern PH_API void phAsyncDevelopFilmRegion(PHuint64 engineId, PHuint64 frameId,
                                            PHuint32 xPx, PHuint32 yPx,
                                            PHuint32 widthPx, PHuint32 heightPx);

#ifdef __cplusplus
}
#endif