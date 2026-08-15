#pragma once

#include <CEngine/ph_c_core_types.h>

#include <Common/primitive_type.h>

#include <array>
#include <bit>
#include <cstddef>
#include <limits>
#include <span>

namespace ph::cli
{

/*! @brief Non-owning view of one image update sent to PhotonBlend.

@code
[x: uint32][y: uint32][width: uint32][height: uint32][channels: uint32]
[pixel 0: channels * float32][pixel 1: channels * float32]...
@endcode

The header is 20 bytes. All values are little-endian. `(x, y)` is the lower-left pixel of the
update. Channels are interleaved in caller-provided order. Pixels run left-to-right and then rows
run bottom-to-top. The pixel payload contains `width * height * channels * 4` bytes.

The frame stream ends with
[0: uint32][0: uint32][0: uint32][0: uint32][0: uint32] and no pixel payload.

The full-frame storage must outlive this view and any write using its byte views.
*/
class BlenderFrameDataView final
{
public:
	using ByteView = std::span<const std::byte>;

	static_assert(
		sizeof(PhFloat32) == 4 &&
		std::numeric_limits<PhFloat32>::is_iec559 &&
		std::endian::native == std::endian::little,
		"Blender frame streaming requires little-endian uint32 and IEEE 754 float32.");

	BlenderFrameDataView(
		const PhFloat32* fullFrameData,
		uint32 frameWidthPx,
		uint32 frameHeightPx,
		uint32 numChannels,
		const PhFrameRegionInfo& region);

	/*! @brief Get the update header bytes.
	*/
	ByteView getHeaderBytes() const;

	/*! @brief Get the header that marks the end of the frame stream.
	*/
	static ByteView getEndHeaderBytes();

	/*! @brief Get the number of pixel rows in the update.
	*/
	uint32 numRows() const;

	/*! @brief Get one contiguous row of update pixels.
	*/
	ByteView getRowBytes(uint32 rowIndex) const;

private:
	std::array<uint32, 5> m_header;
	std::span<const PhFloat32> m_fullFrameData;
	uint32 m_frameWidthPx;
	uint32 m_regionXPx;
	uint32 m_regionYPx;
	uint32 m_regionWidthPx;
	uint32 m_regionHeightPx;
	uint32 m_numChannels;
};

inline BlenderFrameDataView::BlenderFrameDataView(
	const PhFloat32* const fullFrameData,
	const uint32 frameWidthPx,
	const uint32 frameHeightPx,
	const uint32 numChannels,
	const PhFrameRegionInfo& region)

	: m_header          {region.xPx, region.yPx, region.widthPx, region.heightPx, numChannels}
	, m_fullFrameData   (fullFrameData, static_cast<std::size_t>(frameWidthPx) * frameHeightPx * numChannels)
	, m_frameWidthPx    (frameWidthPx)
	, m_regionXPx       (region.xPx)
	, m_regionYPx       (region.yPx)
	, m_regionWidthPx   (region.widthPx)
	, m_regionHeightPx  (region.heightPx)
	, m_numChannels     (numChannels)
{}

inline BlenderFrameDataView::ByteView BlenderFrameDataView::getHeaderBytes() const
{
	return std::as_bytes(std::span(m_header));
}

inline BlenderFrameDataView::ByteView BlenderFrameDataView::getEndHeaderBytes()
{
	static constexpr std::array<uint32, 5> header{};
	return std::as_bytes(std::span(header));
}

inline uint32 BlenderFrameDataView::numRows() const
{
	return m_regionHeightPx;
}

inline BlenderFrameDataView::ByteView BlenderFrameDataView::getRowBytes(const uint32 rowIndex) const
{
	const std::size_t rowBegin =
		(static_cast<std::size_t>(m_regionYPx + rowIndex) * m_frameWidthPx + m_regionXPx) * m_numChannels;
	const std::size_t rowSize =
		static_cast<std::size_t>(m_regionWidthPx) * m_numChannels;
	return std::as_bytes(m_fullFrameData.subspan(rowBegin, rowSize));
}

}// end namespace ph::cli
