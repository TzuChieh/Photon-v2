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

typedef int8_t         PhInt8;
typedef uint8_t        PhUInt8;
typedef int16_t        PhInt16;
typedef uint16_t       PhUInt16;
typedef int32_t        PhInt32;
typedef uint32_t       PhUInt32;
typedef int64_t        PhInt64;
typedef uint64_t       PhUInt64;
typedef float          PhFloat32;
typedef double         PhFloat64;
typedef char           PhChar;
typedef unsigned char  PhUChar;
typedef int32_t        PhBool;
typedef size_t         PhSize;

#define PH_TRUE  1
#define PH_FALSE 0

typedef enum PhFrameFormat
{
	PH_EXR_IMAGE,
	PH_RGBA32F
} PhBufferFormat;

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
	PhInt64   integers[PH_NUM_RENDER_STATE_INTEGERS];
	PhFloat32 reals[PH_NUM_RENDER_STATE_REALS];
};

// HACK
struct PHObservableRenderData
{
	PhChar layers[PH_NUM_RENDER_LAYERS][PH_MAX_NAME_LENGTH + 1];
	PhChar integers[PH_NUM_RENDER_STATE_INTEGERS][PH_MAX_NAME_LENGTH + 1];
	PhChar reals[PH_NUM_RENDER_STATE_REALS][PH_MAX_NAME_LENGTH + 1];
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

extern PH_API void phConfigCoreResourceDirectory(const PhChar* directory);

extern PH_API PhBool phInit();
extern PH_API PhBool phExit();

///////////////////////////////////////////////////////////////////////////////
// Core Operations
//

// TODO: remove the word "film" from develop functions

/*! @brief Creates an engine.
 */
extern PH_API void phCreateEngine(PhUInt64* out_engineId, const PhUInt32 numRenderThreads);

extern PH_API void phSetNumRenderThreads(PhUInt64 engineId, const PhUInt32 numRenderThreads);
extern PH_API void phEnterCommand(PhUInt64 engineId, const PhChar* commandFragment);
extern PH_API PhBool phLoadCommands(PhUInt64 engineId, const PhChar* filePath);
extern PH_API void phRender(PhUInt64 engineId);

// TODO: documentation
extern PH_API void phUpdate(PhUInt64 engineId);

extern PH_API void phGetRenderDimension(PhUInt64 engineId, PhUInt32* out_widthPx, PhUInt32* out_heightPx);

// HACK
extern PH_API void phGetObservableRenderData(
	PhUInt64                       engineId,
	struct PHObservableRenderData* out_data);

extern PH_API void phDeleteEngine(PhUInt64 engineId);
extern PH_API void phSetWorkingDirectory(PhUInt64 engineId, const PhChar* workingDirectory);

// REFACTOR: rename aquire to retrieve
extern PH_API void phAquireFrame(PhUInt64 engineId, PhUInt64 channelIndex, PhUInt64 frameId);
extern PH_API void phAquireFrameRaw(PhUInt64 engineId, PhUInt64 channelIndex, PhUInt64 frameId);

///////////////////////////////////////////////////////////////////////////////
// Frame Operations
//

extern PH_API void  phCreateFrame(PhUInt64* out_frameId, PhUInt32 widthPx, PhUInt32 heightPx);
extern PH_API void  phGetFrameDimension(PhUInt64 frameId, PhUInt32* out_widthPx, PhUInt32* out_heightPx);
extern PH_API void  phGetFrameRgbData(PhUInt64 frameId, const PhFloat32** out_data);
extern PH_API void  phDeleteFrame(PhUInt64 frameId);
extern PH_API PhBool phLoadFrame(PhUInt64 frameId, const PhChar* filePath);

/*! @brief Save a frame to the filesystem.
*/
extern PH_API int phSaveFrame(PhUInt64 frameId, const PhChar* filePath);

/*! @brief Save a frame to a buffer.
@param saveInBigEndian If applicable to the format, specifies whether the result is saved in big endian.
This is useful, for example, transferring the data through the Internet, where big-endian is the
standard byte order.
*/
extern PH_API PhBool phSaveFrameToBuffer(
	PhUInt64 frameId,
	PhUInt64 bufferId,
	PhFrameFormat formatToSaveIn,
	PhBool saveInBigEndian);

extern PH_API void phFrameOpAbsDifference(PhUInt64 frameAId, PhUInt64 frameBId, PhUInt64 resultFrameId);
extern PH_API PhFloat32 phFrameOpMSE(PhUInt64 expectedFrameId, PhUInt64 estimatedFramIde);

///////////////////////////////////////////////////////////////////////////////
// Miscellaneous Operations
//

extern PH_API void phCreateBuffer(PhUInt64* out_bufferId);
extern PH_API void phGetBufferBytes(PhUInt64 bufferId, const PhUChar** out_bytesPtr, PhSize* out_numBytes);
extern PH_API void phDeleteBuffer(PhUInt64 bufferId);

///////////////////////////////////////////////////////////////////////////////
// Asynchronous Operations
//

// TODO: async queries should tolerate invalid operations such as being called
// after underlying resource is already deleted, this can ease the burden of 
// callers

extern PH_API void phAsyncGetRendererStatistics(
	PhUInt64                 engineId,
	PhFloat32*               out_percentageProgress,
	PhFloat32*               out_samplesPerSecond);

// HACK
extern PH_API void phAsyncGetRendererState(
	PhUInt64                 engineId,
	struct PHRenderState*    out_state);

extern PH_API PhBool phAsyncPollUpdatedFrameRegion(
	PhUInt64                 engineId,
	PhUInt32*                out_xPx,
	PhUInt32*                out_yPx,
	PhUInt32*                out_widthPx,
	PhUInt32*                out_heightPx);

extern PH_API void phAsyncPeekFrame(
	PhUInt64                 engineId,
	PhUInt64                 channelIndex,
	PhUInt32                 xPx,
	PhUInt32                 yPx,
	PhUInt32                 widthPx,
	PhUInt32                 heightPx,
	PhUInt64                 frameId);

extern PH_API void phAsyncPeekFrameRaw(
	PhUInt64                 engineId,
	PhUInt64                 channelIndex,
	PhUInt32                 xPx,
	PhUInt32                 yPx,
	PhUInt32                 widthPx,
	PhUInt32                 heightPx,
	PhUInt64                 frameId);

#ifdef __cplusplus
}
#endif
