#pragma once

#include "Engine/Frame/picture_basics.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Frame/frame_fwd.h"
#include "Engine/Utility/TSpan.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <memory>

namespace ph
{

/*! @brief Stores a picture.

This layout is intended for file I/O and common image manipulations. Use `TFrame` for runtime frame
operations.
*/
class PictureData final
{
public:
	/*! @brief Create an empty picture. */
	PictureData();

	/*! @brief Create a picture with uninitialized storage.
	Use `setPixels()` or a mutable storage view to fill it.
	*/
	PictureData(
		math::Vector2S sizePx,
		std::size_t numPicComponents,
		EPicturePixelComponent componentType);

	/*! @brief Create a picture by copying @p pixelData. */
	template<typename PixelData>
	PictureData(
		math::Vector2S sizePx,
		std::size_t numPicComponents,
		EPicturePixelComponent componentType,
		const PixelData* pixelData,
		std::size_t pixelDataSize);

	/*! @brief Create a picture by copying pixels from @p frame.
	The two objects cannot share storage because `TFrame` may use a different memory layout.
	*/
	template<typename FrameComponent, std::size_t N>
	explicit PictureData(const TFrame<FrameComponent, N>& frame);

	PictureData(PictureData&& other);

	const math::Vector2S& getSizePx() const;
	std::size_t getWidthPx() const;
	std::size_t getHeightPx() const;
	std::size_t numComponents() const;
	EPicturePixelComponent getComponentType() const;
	TSpan<std::byte> getBytes();
	TSpanView<std::byte> getBytes() const;

	/*! @brief View the storage as components of its declared component type.
	`Component` must match `getComponentType()`.
	*/
	///@{
	template<typename Component>
	TSpan<Component> components();

	template<typename Component>
	TSpanView<Component> getComponents() const;
	///@}

	bool isEmpty() const;

	/*! @brief Copy raw pixel data into the picture storage.

	`PixelData` may pack multiple components into one element. For example, an `int32` can hold four
	8-bit RGBA components. The input byte count must match the storage size.
	*/
	template<typename PixelData>
	void setPixels(
		const PixelData* pixelData,
		std::size_t numPixelDataElements);

	/*! @brief Copy pixels into a new frame.

	Components present in both formats are converted. Extra picture components are ignored. If the
	frame has more components, the additional components are not initialized.
	*/
	template<typename FrameComponent, std::size_t N>
	TFrame<FrameComponent, N> toFrame() const;
	// TODO: Add an option to allow or reject lossy conversion.

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

#include "Engine/Frame/PictureData.ipp"
