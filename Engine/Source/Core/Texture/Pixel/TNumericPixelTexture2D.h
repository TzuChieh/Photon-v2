#pragma once

#include "Core/Texture/Pixel/TPixelTexture2D.h"
#include "Common/assertion.h"
#include "Common/primitive_type.h"
#include "Core/Texture/SampleLocation.h"
#include "Math/TArithmeticArray.h"

#include <cstddef>
#include <algorithm>

namespace ph
{

template<typename T, std::size_t N>
class TNumericPixelTexture2D : public TPixelTexture2D<math::TArithmeticArray<T, N>>
{
public:
	explicit TNumericPixelTexture2D(const std::shared_ptr<PixelBuffer2D>& pixelBuffer);

	TNumericPixelTexture2D(
		const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
		pixel_texture::ESampleMode            sampleMode,
		pixel_texture::EWrapMode              wrapModeS,
		pixel_texture::EWrapMode              wrapModeT);

	TNumericPixelTexture2D(
		const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
		pixel_texture::ESampleMode            sampleMode,
		pixel_texture::EWrapMode              wrapModeS,
		pixel_texture::EWrapMode              wrapModeT,
		std::size_t                           pixelIndexOffset);

	void sample(
		const SampleLocation&         sampleLocation, 
		math::TArithmeticArray<T, N>* out_value) const override;

private:
	std::size_t m_pixelIndexOffset;
};

// In-header Implementations:

template<typename T, std::size_t N>
inline TNumericPixelTexture2D<T, N>::TNumericPixelTexture2D(const std::shared_ptr<PixelBuffer2D>& pixelBuffer) :
	
	TPixelTexture2D<math::TArithmeticArray<T, N>>(pixelBuffer),

	m_pixelIndexOffset(0)
{}

template<typename T, std::size_t N>
inline TNumericPixelTexture2D<T, N>::TNumericPixelTexture2D(
	const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
	const pixel_texture::ESampleMode      sampleMode,
	const pixel_texture::EWrapMode        wrapModeS,
	const pixel_texture::EWrapMode        wrapModeT) :

	TNumericPixelTexture2D(
		pixelBuffer,
		sampleMode,
		wrapModeS,
		wrapModeT,
		0)
{}

template<typename T, std::size_t N>
inline TNumericPixelTexture2D<T, N>::TNumericPixelTexture2D(
	const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
	const pixel_texture::ESampleMode      sampleMode,
	const pixel_texture::EWrapMode        wrapModeS,
	const pixel_texture::EWrapMode        wrapModeT,
	const std::size_t                     pixelIndexOffset) :

	TPixelTexture2D<math::TArithmeticArray<T, N>>(
		pixelBuffer,
		sampleMode,
		wrapModeS,
		wrapModeT),

	m_pixelIndexOffset(pixelIndexOffset)
{}

template<typename T, std::size_t N>
inline void TNumericPixelTexture2D<T, N>::sample(
	const SampleLocation&               sampleLocation,
	math::TArithmeticArray<T, N>* const out_value) const
{
	PH_ASSERT(out_value);

	const pixel_buffer::TPixel<float64> sampledPixel = this->samplePixelBuffer(math::Vector2D(sampleLocation.uv()));

	// Number of sampled values is limited by either `N` or the dimension of sampled pixel
	const auto minBufferSize = std::min(N, sampledPixel.numValues());

	out_value->set(0);
	for(std::size_t i = m_pixelIndexOffset; i < minBufferSize; ++i)
	{
		(*out_value)[i] = static_cast<T>(sampledPixel[i]);
	}
}

}// end namespace ph
