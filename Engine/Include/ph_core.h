#pragma once

/*! @file

@brief Contains main C APIs of the render engine.

To correctly use Photon-v2 API, please read the following notes:

- phInit() and phExit() must be called before and after the use of the API, and
  should be called on the same thread.

- phCreate/Delete<X>() and phAsync<X>() functions can be used in a multithreaded
  environment, namely, they are thread-safe. An exception would be that create 
  and delete should be called from the same thread.

- Resources created by phCreate<X>() cannot be manipulated concurrently. Any
  function requiring some resource ID inputs (except phAsync<X>() functions) is
  considered a resource manipulating operation on those resources.

*/

// Note that this header should always be compatible to C.

// HACK
//#define PH_EXPORT_API

// Checking the compiling environment.
//#if defined(_MSC_VER)
///**************************************************** Microsoft Visual Studio */
//
//	// Note: The use of dllexport implies a definition, while dllimport implies a declaration. To force a declaration,
//	// an "extern" keyword with dllexport is needed; otherwise, a definition is implied.
//
//	#ifdef PH_EXPORT_API
//		#define PH_API __declspec(dllexport)
//	#else
//		#define PH_API __declspec(dllimport)
//	#endif
//
//#else
///************************************************************ other compilers */
//
//	// TODO
//	#ifdef EXPORT_PHOTON_API
//		#define PH_API
//	#else
//		#define PH_API
//	#endif
//
//#endif
// end compiling environment check

// FIXME: sort of hacked
#define PH_API

// primitive data types

#include <stdint.h>
#include <stddef.h>

typedef char           PHint8;
typedef unsigned char  PHuint8;
typedef int            PHint32;
typedef unsigned int   PHuint32;
typedef int64_t        PHint64;
typedef uint64_t       PHuint64;
typedef float          PHfloat32;
typedef double         PHfloat64;
typedef char           PHchar;

#define PH_TRUE  1
#define PH_FALSE 0

// HACK
enum PH_EATTRIBUTE
{
	LIGHT_ENERGY,
	NORMAL,
	DEPTH
};

// HACK
#define PH_NUM_RENDER_LAYERS         4
#define PH_NUM_RENDER_STATE_INTEGERS 4
#define PH_NUM_RENDER_STATE_REALS    4
#define PH_MAX_NAME_LENGTH           128

// HACK
struct PHRenderState
{
	PHint64   integers[PH_NUM_RENDER_STATE_INTEGERS];
	PHfloat32 reals[PH_NUM_RENDER_STATE_REALS];
};

// HACK
struct PHObservableRenderData
{
	PHchar layers[PH_NUM_RENDER_LAYERS][PH_MAX_NAME_LENGTH + 1];
	PHchar integers[PH_NUM_RENDER_STATE_INTEGERS][PH_MAX_NAME_LENGTH + 1];
	PHchar reals[PH_NUM_RENDER_STATE_REALS][PH_MAX_NAME_LENGTH + 1];
};

// HACK
enum PH_ERenderStateType
{
	INTEGER,
	REAL
};

#define PH_FILM_REGION_STATUS_INVALID  -1
#define PH_FILM_REGION_STATUS_UPDATING 1
#define PH_FILM_REGION_STATUS_FINISHED 2

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// starting and exiting Photon
//

extern PH_API void phConfigCoreResourceDirectory(const PHchar* directory);

extern PH_API int phInit();
extern PH_API int phExit();

///////////////////////////////////////////////////////////////////////////////
// Core Operations
//

// TODO: remove the word "film" from develop functions

/*! @brief Creates an engine.
 */
extern PH_API void phCreateEngine(PHuint64* out_engineId, const PHuint32 numRenderThreads);

extern PH_API void phSetNumRenderThreads(PHuint64 engineId, const PHuint32 numRenderThreads);
extern PH_API void phEnterCommand(PHuint64 engineId, const PHchar* commandFragment);
extern PH_API void phRender(PHuint64 engineId);

// TODO: documentation
extern PH_API void phUpdate(PHuint64 engineId);

extern PH_API void phGetRenderDimension(PHuint64 engineId, PHuint32* out_widthPx, PHuint32* out_heightPx);

// HACK
extern PH_API void phGetObservableRenderData(
	PHuint64                       engineId,
	struct PHObservableRenderData* out_data);

extern PH_API void phDeleteEngine(PHuint64 engineId);
extern PH_API void phSetWorkingDirectory(PHuint64 engineId, const PHchar* workingDirectory);

// REFACTOR: rename aquire to retrieve
extern PH_API void phAquireFrame(PHuint64 engineId, PHuint64 channelIndex, PHuint64 frameId);
extern PH_API void phAquireFrameRaw(PHuint64 engineId, PHuint64 channelIndex, PHuint64 frameId);

///////////////////////////////////////////////////////////////////////////////
// Frame Operations
//

extern PH_API void  phCreateFrame(PHuint64* out_frameId, PHuint32 widthPx, PHuint32 heightPx);
extern PH_API void  phGetFrameDimension(PHuint64 frameId, PHuint32* out_widthPx, PHuint32* out_heightPx);
extern PH_API void  phGetFrameRgbData(PHuint64 frameId, const PHfloat32** out_data);
extern PH_API void  phDeleteFrame(PHuint64 frameId);
extern PH_API int   phLoadFrame(PHuint64 frameId, const PHchar* filePath);
extern PH_API int   phSaveFrame(PHuint64 frameId, const PHchar* filePath);
extern PH_API void  phFrameOpAbsDifference(PHuint64 frameAId, PHuint64 frameBId, PHuint64 resultFrameId);
extern PH_API float phFrameOpMSE(PHuint64 expectedFrameId, PHuint64 estimatedFramIde);

///////////////////////////////////////////////////////////////////////////////
// Miscellaneous Operations
//

extern PH_API void phCreateBuffer(PHuint64* out_bufferId);
extern PH_API void phGetBufferBytes(PHuint64 bufferId, const unsigned char** out_bytesPtr, size_t* out_numBytes);
extern PH_API void phDeleteBuffer(PHuint64 bufferId);

///////////////////////////////////////////////////////////////////////////////
// Asynchronous Operations
//

// TODO: async queries should tolerate invalid operations such as being called
// after underlying resource is already deleted, this can ease the burden of 
// callers

extern PH_API void phAsyncGetRendererStatistics(
	PHuint64                 engineId,
	PHfloat32*               out_percentageProgress,
	PHfloat32*               out_samplesPerSecond);

// HACK
extern PH_API void phAsyncGetRendererState(
	PHuint64                 engineId,
	struct PHRenderState*    out_state);

extern PH_API int  phAsyncPollUpdatedFrameRegion(
	PHuint64                 engineId,
	PHuint32*                out_xPx,
	PHuint32*                out_yPx,
	PHuint32*                out_widthPx,
	PHuint32*                out_heightPx);

extern PH_API void phAsyncPeekFrame(
	PHuint64                 engineId, 
	PHuint64                 channelIndex,
	PHuint32                 xPx, 
	PHuint32                 yPx,
	PHuint32                 widthPx, 
	PHuint32                 heightPx,
	PHuint64                 frameId);

extern PH_API void phAsyncPeekFrameRaw(
	PHuint64                 engineId,
	PHuint64                 channelIndex,
	PHuint32                 xPx,
	PHuint32                 yPx,
	PHuint32                 widthPx,
	PHuint32                 heightPx,
	PHuint64                 frameId);

#ifdef __cplusplus
}
#endif