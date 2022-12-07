#pragma once

#include "Frame/picture_basics.h"
#include "Math/TVector2.h"
#include "Common/primitive_type.h"
#include "Frame/frame_fwd.h"

#include <cstddef>
#include <memory>

namespace ph
{

/*!
Data storage of a picture. Has limited ability on pixel data manipulation. It is recommended
to convert to `TFrame` via `toFrame()` for operations on pixel data.
*/
class PictureData final
{
public:
	/*! @brief Creates an empty picture.
	*/
	PictureData();

	/*! @brief Creates a picture with allocated buffer.
	Use setPixels(const PixelData*, std::size_t) to supply pixel data.
	*/
	PictureData(
		math::Vector2S sizePx,
		std::size_t numPicComponents,
		EPicturePixelComponent componentType);

	/*! @brief Creates a picture filled with pixel data.
	*/
	template<typename PixelData>
	PictureData(
		math::Vector2S sizePx,
		std::size_t numPicComponents,
		EPicturePixelComponent componentType,
		const PixelData* pixelData,
		std::size_t pixelDataSize);

	PictureData(PictureData&& other);

	const math::Vector2S& getSizePx() const;
	std::size_t getWidthPx() const;
	std::size_t getHeightPx() const;
	std::size_t numComponents() const;
	EPicturePixelComponent getComponentType() const;
	std::byte* getData();
	const std::byte* getData() const;
	std::size_t numBytesInData() const;
	bool isEmpty() const;

	template<typename PixelData>
	void setPixels(
		const PixelData* pixelData,
		std::size_t numPixelDataElements);

	// TODO: option for allow/disallow lossy conversion?
	template<typename FrameComponent, std::size_t N>
	TFrame<FrameComponent, N> toFrame() const;

	PictureData& operator = (PictureData&& rhs);

private:
	template<typename PictureComponent, typename FrameComponent, std::size_t N>
	TFrame<FrameComponent, N> pictureToFrame() const;

	template<typename PictureComponent>
	std::size_t getByteIndex(std::size_t xPx, std::size_t yPx, std::size_t componentIndex) const;

	math::Vector2S m_sizePx;
	std::size_t m_numComponents;
	EPicturePixelComponent m_componentType;
	std::unique_ptr<std::byte[]> m_data;
	std::size_t m_numBytesInData;
};

}// end namespace ph

#include "Frame/PictureData.ipp"
