#pragma once

#include "Core/Texture/Pixel/PixelBuffer2D.h"
#include "Frame/TFrame.h"
#include "Math/TArithmeticArray.h"

#include <cstddef>
#include <utility>
#include <type_traits>

namespace ph
{

template<typename T, std::size_t N>
class TFrameBuffer2D : public PixelBuffer2D
{
	static_assert(N <= pixel_buffer::MAX_PIXEL_ELEMENTS,
		"Number of pixel components exceeded allowed number");

public:
	explicit TFrameBuffer2D(TFrame<T, N> frame);

	pixel_buffer::TPixel<float64> fetchPixel(math::TVector2<uint32> xy, std::size_t mipLevel) const override;
	std::size_t estimateMemoryUsageBytes() const override;

	/*! @brief Directly get pixel value stored in the frame.
	@return Pixel value of type `TFrame<T, N>::Pixel`. Note that LDR values (typically stored as 8-bit types) 
	will be normalized to [0, 1], and in such case the pixel type will be `TFrame<float32, N>::Pixel`.
	*/
	auto getFramePixel(math::TVector2<uint32> xy, std::size_t mipLevel) const;

private:
	TFrame<T, N> m_frame;
};

// In-header Implementations:

template<typename T, std::size_t N>
inline TFrameBuffer2D<T, N>::TFrameBuffer2D(TFrame<T, N> frame) :

	PixelBuffer2D(
		frame.getSizePx(),
		N),

	m_frame(std::move(frame))
{}

template<typename T, std::size_t N>
inline pixel_buffer::TPixel<float64> TFrameBuffer2D<T, N>::fetchPixel(const math::TVector2<uint32> xy, const std::size_t mipLevel) const
{
	// Directly cast all value types to float64
	return typename TFrame<float64, N>::PixelType(getFramePixel(xy, mipLevel)).toArray();
}

template<typename T, std::size_t N>
inline auto TFrameBuffer2D<T, N>::getFramePixel(const math::TVector2<uint32> xy, const std::size_t mipLevel) const
{
	const typename TFrame<T, N>::PixelType framePixel = m_frame.getPixel(xy);

	// TODO: this should be a configurable option for each type; or some fixed policy
	// Convert from [0, 255] to [0, 1] for LDR values
	if constexpr(std::is_same_v<T, uint8>)
	{
		return typename TFrame<float32, N>::PixelType(framePixel) / 255.0f;
	}
	// For rest of the types, no conversion is made
	else
	{
		return framePixel;
	}
}

template<typename T, std::size_t N>
inline std::size_t TFrameBuffer2D<T, N>::estimateMemoryUsageBytes() const
{
	const auto baseUsage  = PixelBuffer2D::estimateMemoryUsageBytes();
	const auto frameUsage = sizeof(TFrame<T, N>);

	// Estimate usage of frame internal buffer
	const auto numPixels   = math::Vector2S(m_frame.getSizePx()).product();
	const auto bufferUsage = numPixels * (sizeof(T) * N);

	return baseUsage + frameUsage + bufferUsage;
}

}// end namespace ph
