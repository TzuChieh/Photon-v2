#pragma once

/*! @file

@brief Contains main C APIs of the render engine.

To correctly use Photon-v2 API, please read the following notes:

- `phInit()` and `phExit()` must be called before and after the use of the API, and
  should be called on the same thread.

- `phCreate/Delete<X>()` and `phAsync<X>()` functions can be used in a multithreaded
  environment. They are thread-safe provided the following conditions are met:
  * `phDelete<X>()` should be called from the same thread its corresponding
    `phCreate<X>()` was called from.
  * `phAsync<X>()` cannot be called during `phUpdate()` (memory effects must be made
    visible to the calling thread).

- Resources created by `phCreate<X>()` cannot be manipulated concurrently. Any
  function requiring some resource ID inputs (except `phAsync<X>()` functions) is
  considered a resource manipulating operation on those resources.

*/

// Note that this header should always be compatible to C.

// Optional manual API export toggle (currently disabled).
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

#include "CEngine/ph_c_core_types.h"

typedef struct PhRenderObservationInfo
{
	PhSize numLayers;
	PhSize numIntegerStats;
	PhSize numRealStats;
} PhRenderObservationInfo;

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// starting and exiting Photon
//

extern PH_API PhResult phInit();
extern PH_API PhResult phExit();

///////////////////////////////////////////////////////////////////////////////
// Core Operations
//

// TODO: remove the word "film" from develop functions

/*! @brief Creates a render session.
 */
extern PH_API void phCreateSession(PhUInt64* out_sessionId, PhUInt32 numRenderThreads);

extern PH_API void phSetNumRenderThreads(PhUInt64 sessionId, PhUInt32 numRenderThreads);
extern PH_API void phEnterCommand(PhUInt64 sessionId, const PhChar* commandFragment);
extern PH_API PhResult phLoadCommands(PhUInt64 sessionId, const PhChar* filePath);
extern PH_API void phRender(PhUInt64 sessionId);

// TODO: documentation
extern PH_API void phUpdate(PhUInt64 sessionId);

/*! @brief Gets render dimension.
Writes `0` to outputs on failure.
*/
extern PH_API void phGetRenderDimension(PhUInt64 sessionId, PhUInt32* out_widthPx, PhUInt32* out_heightPx);

/*! @brief Gets counts of observable render outputs and stat channels.
Writes `0` to all fields in @p out_info on failure.
 */
extern PH_API void phGetRenderObservationInfo(
	PhUInt64 sessionId,
	PhRenderObservationInfo* out_info);

/*! @brief Gets render layer display name.
@param sessionId Render session ID.
@param layerIndex Layer index.
@param out_name Output buffer for storing the name. Can be `nullptr` for size query only.
@param out_nameLength Actual name length in bytes including NUL terminator if not `nullptr`.
If @p out_name is provided, caller is responsible for ensuring the buffer is large enough.
@return `PH_OK` on success. Output is only available if `PH_OK` is returned.
`PH_ERROR_INVALID_ARGUMENT` if both @p out_name and @p out_nameLength are `nullptr`.
`PH_ERROR_NOT_FOUND` if render observation info is unavailable.
`PH_ERROR_OUT_OF_RANGE` if @p layerIndex is out of range.
*/
extern PH_API PhResult phGetRenderLayerName(
	PhUInt64 sessionId,
	PhInt32 layerIndex,
	PhChar* out_name,
	PhSize* out_nameLength);

/*! @brief Gets integer render stat display name.
Uses the same contract as phGetRenderLayerName().
*/
extern PH_API PhResult phGetRenderIntegerStatName(
	PhUInt64 sessionId,
	PhInt32 statIndex,
	PhChar* out_name,
	PhSize* out_nameLength);

/*! @brief Gets real-number render stat display name.
Uses the same contract as phGetRenderLayerName().
*/
extern PH_API PhResult phGetRenderRealStatName(
	PhUInt64 sessionId,
	PhInt32 statIndex,
	PhChar* out_name,
	PhSize* out_nameLength);

extern PH_API void phDeleteSession(PhUInt64 sessionId);
extern PH_API void phSetWorkingDirectory(PhUInt64 sessionId, const PhChar* workingDirectory);

extern PH_API PhResult phRetrieveFrame(PhUInt64 sessionId, PhInt32 layerIndex, PhUInt64 frameId);
extern PH_API PhResult phRetrieveFrameRaw(PhUInt64 sessionId, PhInt32 layerIndex, PhUInt64 frameId);

///////////////////////////////////////////////////////////////////////////////
// Frame Operations
//

extern PH_API void  phCreateFrame(PhUInt64* out_frameId, PhUInt32 widthPx, PhUInt32 heightPx);
extern PH_API void  phGetFrameDimension(PhUInt64 frameId, PhUInt32* out_widthPx, PhUInt32* out_heightPx);
extern PH_API void  phGetFrameRgbData(PhUInt64 frameId, const PhFloat32** out_data);
extern PH_API void  phDeleteFrame(PhUInt64 frameId);
extern PH_API PhResult phLoadFrame(PhUInt64 frameId, const PhChar* filePath);

/*! @brief Save a frame to the filesystem.
*/
extern PH_API PhResult phSaveFrame(
	PhUInt64 frameId, 
	const PhChar* filePath,
	const PhFrameSaveInfo* saveInfo);

/*! @brief Save a frame to a buffer.
@param saveInBigEndian If applicable to the format, specifies whether the result is saved in big endian.
This is useful, for example, transferring the data through the Internet, where big-endian is the
standard byte order.
*/
extern PH_API PhResult phSaveFrameToBuffer(
	PhUInt64 frameId,
	PhUInt64 bufferId,
	PhBufferFormat format,
	const PhFrameSaveInfo* saveInfo);

extern PH_API void phFrameOpAbsDifference(PhUInt64 frameAId, PhUInt64 frameBId, PhUInt64 resultFrameId);
extern PH_API PhFloat32 phFrameOpMSE(PhUInt64 expectedFrameId, PhUInt64 estimatedFramIde);

///////////////////////////////////////////////////////////////////////////////
// General Buffer Operations
//

extern PH_API void phCreateBuffer(PhUInt64* out_bufferId);
extern PH_API void phGetBufferBytes(PhUInt64 bufferId, const PhUChar** out_bytesPtr, PhSize* out_numBytes);
extern PH_API void phDeleteBuffer(PhUInt64 bufferId);

///////////////////////////////////////////////////////////////////////////////
// Asynchronous Operations
//

/*! @brief Gets render progress asynchronously.
Writes zeroed progress if query fails.
*/
extern PH_API void phAsyncGetRenderProgress(PhUInt64 sessionId, PhRenderProgress* out_progress);

// TODO: async queries should tolerate invalid operations such as being called
// after underlying resource is already deleted, this can ease the burden of 
// callers

/*! @brief Gets render statistics asynchronously.
Writes `0` to both outputs if query fails.
*/
extern PH_API void phAsyncGetRenderStatistics(
	PhUInt64                 sessionId,
	PhFloat32*               out_percentageProgress,
	PhFloat32*               out_samplesPerSecond);

/*! @brief Polls for a single changed region during engine runtime.
On failure or no update, @p out_regionInfo will be set to an invalid region
(`PH_FRAME_REGION_STATUS_INVALID`).
*/
extern PH_API void phAsyncPollUpdatedFrameRegion(
	PhUInt64 sessionId,
	PhFrameRegionInfo* out_regionInfo);

/*! @brief Polls for multiple changed regions during engine runtime.
Returns `0` if query fails.
*/
extern PH_API PhSize phAsyncPollUpdatedFrameRegions(
	PhUInt64 sessionId,
	PhUInt64 bufferId,
	PhFrameRegionInfo* out_regionInfos,
	PhSize regionInfoSize);

/*! @brief Polls for multiple changed regions during engine runtime.
@param out_regionInfos Output buffer for storing the updated regions. The size of the buffer also
determines the maximum number of regions that will participate in the merging process.
@param mergeSize Number of regions to merge to a single unit. To obtain a single region, you can
specify a number greater or equal to the size of the output buffer.
Returns `0` if query fails.
*/
extern PH_API PhSize phAsyncPollMergedUpdatedFrameRegions(
	PhUInt64 sessionId,
	PhUInt64 bufferId,
	PhSize mergeSize,
	PhFrameRegionInfo* out_regionInfos,
	PhSize regionInfoSize);

extern PH_API void phAsyncPeekFrame(
	PhUInt64 sessionId,
	PhInt32 layerIndex,
	PhUInt32 xPx,
	PhUInt32 yPx,
	PhUInt32 widthPx,
	PhUInt32 heightPx,
	PhUInt64 frameId);

extern PH_API void phAsyncPeekFrameRaw(
	PhUInt64 sessionId,
	PhInt32 layerIndex,
	PhUInt32 xPx,
	PhUInt32 yPx,
	PhUInt32 widthPx,
	PhUInt32 heightPx,
	PhUInt64 frameId);

#ifdef __cplusplus
}
#endif
