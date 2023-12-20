#pragma once

#include "ph_c_core_types.h"

#include <Utility/TSpan.h>
#include <Core/Renderer/ERegionStatus.h>

#include <cstddef>
#include <bit>

namespace ph { class PictureMeta; }
namespace ph { class ByteBuffer; }
namespace ph { class RenderRegionStatus; }

namespace ph
{

bool is_reversing_bytes_needed(PhEndian desiredEndianness);

PhFrameRegionStatus to_frame_region_status(ERegionStatus regionStatus);
void to_frame_region_info(const RenderRegionStatus& regionStatus, PhFrameRegionInfo* out_regionInfo);

PictureMeta make_picture_meta(const PhFrameSaveInfo& saveInfo);

/*! @brief Make an array of default constructed objects of type `T`.
@tparam T Type for the objects created. Must be default constructible and trivially destructible.
@param arraySize Number of array elements.
@param buffer The buffer object to back the array. The write head will move past the region
for the array.
@param allowBufferGrowth Making an array from the buffer may invalidate any previously returned pointers
if the buffer is allowed to grow. If `false`, no previously returned pointers are invalidated.
@return Span of the created objects. Empty if not enough storage is left.
*/
template<typename T>
TSpan<T> make_array_from_buffer(
	std::size_t numArrayElements,
	ByteBuffer& buffer,
	bool allowBufferGrowth = false);

}// end namespace ph

#include "Api/api_helpers.ipp"
