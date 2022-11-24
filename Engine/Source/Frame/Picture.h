#pragma once

#include "Math/TVector2.h"
#include "Common/primitive_type.h"
#include "Frame/frame_fwd.h"

#include <cstddef>
#include <memory>

namespace ph
{

enum class EPicturePixelComponent
{
	Unspecified = 0,

	PPC_Int8,
	PPC_UInt8,
	PPC_Int16,
	PPC_UInt16,
	PPC_Int32,
	PPC_UInt32,
	PPC_Int64,
	PPC_UInt64,
	PPC_Float16,
	PPC_Float32,
	PPC_Float64
};

class Picture final
{
public:
	template<typename PixelData>
	Picture(
		math::Vector2S sizePx,
		std::size_t numPicComponents,
		EPicturePixelComponent componentType,
		const PixelData* pixelData,
		std::size_t pixelDataSize);

	const math::Vector2S& getSizePx() const;
	std::size_t getWidthPx() const;
	std::size_t getHeightPx() const;
	std::size_t numComponents() const;
	std::byte* getData();
	const std::byte* getData() const;

	template<typename FrameComponent, std::size_t N>
	TFrame<FrameComponent, N> toFrame() const;

private:
	template<typename PictureComponent, typename FrameComponent, std::size_t N>
	TFrame<FrameComponent, N> pictureToFrame() const;

	template<typename PictureComponent>
	std::size_t getByteIndex(std::size_t xPx, std::size_t yPx, std::size_t componentIndex) const;

	static std::size_t numBytesInComponent(EPicturePixelComponent componentType);

	math::Vector2S m_sizePx;
	std::size_t m_numComponents;
	EPicturePixelComponent m_componentType;
	std::unique_ptr<std::byte[]> m_data;
};

}// end namespace ph

#include "Frame/Picture.ipp"
