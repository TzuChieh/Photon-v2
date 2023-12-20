#pragma once

#include "Api/api_helpers.h"

#include <Common/assertion.h>
#include <Common/memory.h>
#include <Utility/ByteBuffer.h>
#include <Core/Renderer/RenderRegionStatus.h>

#include <type_traits>
#include <memory>

namespace ph
{

inline bool is_reversing_bytes_needed(PhEndian desiredEndianness)
{
	constexpr auto native = std::endian::native;

	return 
		(native == std::endian::little && desiredEndianness == PhEndian::PH_BIG_ENDIAN) ||
		(native == std::endian::big && desiredEndianness == PhEndian::PH_LITTLE_ENDIAN);
}

inline PhFrameRegionStatus to_frame_region_status(ERegionStatus regionStatus)
{
	switch(regionStatus)
	{
	case ERegionStatus::Finished:
		return PH_FRAME_REGION_STATUS_FINISHED;

	case ERegionStatus::Updating:
		return PH_FRAME_REGION_STATUS_UPDATING;

	default:
		return PH_FRAME_REGION_STATUS_INVALID;
	}
}

inline void to_frame_region_info(const RenderRegionStatus& regionStatus, PhFrameRegionInfo* out_regionInfo)
{
	PH_ASSERT(out_regionInfo);

	const Region region = regionStatus.getRegion();
	const ERegionStatus status = regionStatus.getStatus();

	out_regionInfo->xPx = static_cast<PhUInt32>(region.getMinVertex().x());
	out_regionInfo->yPx = static_cast<PhUInt32>(region.getMinVertex().y());
	out_regionInfo->widthPx = static_cast<PhUInt32>(region.getWidth());
	out_regionInfo->heightPx = static_cast<PhUInt32>(region.getHeight());
	out_regionInfo->status = to_frame_region_status(status);
}

template<typename T>
inline TSpan<T> make_array_from_buffer(
	std::size_t numArrayElements,
	ByteBuffer& buffer,
	bool allowBufferGrowth)
{
	// To not violate [basic.life] section 8.3 later
	// (https://timsong-cpp.github.io/cppwp/basic.life#8.3)
	using NonConstT = std::remove_const_t<T>;

	static_assert(std::is_default_constructible_v<NonConstT>);
	static_assert(std::is_trivially_destructible_v<NonConstT>);

	const auto alignmentInBytes = alignof(NonConstT);
	const auto arraySizeInBytes = sizeof(NonConstT) * numArrayElements;

	void* ptr = buffer.getBytes().data() + buffer.getWritePosition();
	std::size_t availableBytes = buffer.numBytes() - buffer.getWritePosition();
	void* alignedPtr = std::align(alignmentInBytes, arraySizeInBytes, ptr, availableBytes);

	// Grow the buffer if allowed
	if(!alignedPtr && allowBufferGrowth)
	{
		// Set to a size that the alignment should always work
		buffer.setNumBytes(buffer.getWritePosition() + arraySizeInBytes + alignmentInBytes);

		// Align again
		ptr = buffer.getBytes().data() + buffer.getWritePosition();
		availableBytes = buffer.numBytes() - buffer.getWritePosition();
		alignedPtr = std::align(alignmentInBytes, arraySizeInBytes, ptr, availableBytes);

		PH_ASSERT(alignedPtr);
	}

	// Return empty span if there is not enough space left
	if(!alignedPtr)
	{
		return {};
	}

	PH_ASSERT(alignedPtr);

	// We have a successfully aligned pointer here, update buffer states
	
	// `std::align()` only adjusts `ptr` to the aligned memory location
	std::byte* endPtr = static_cast<std::byte*>(ptr) + arraySizeInBytes;
	buffer.setWritePosition(endPtr - buffer.getBytes().data());

	// `std::align()` only decreases `availableBytes` by the number of bytes used for alignment
	PH_ASSERT_GE(availableBytes, arraySizeInBytes);

	// Prepare the array to return

	// IOC of array of size `numArrayElements`
	NonConstT* const storage = start_implicit_lifetime_as_array<NonConstT>(alignedPtr, arraySizeInBytes);

	// Part of the contract of this function--default construct array elements
	std::uninitialized_default_construct_n(storage, numArrayElements);

	// Potentially do implicit non-const -> const conversion
	return TSpan<NonConstT>(storage, numArrayElements);
}

}// end namespace ph
