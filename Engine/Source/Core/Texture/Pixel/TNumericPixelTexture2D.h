#pragma once

#include "Core/Texture/Pixel/TPixelTexture2D.h"
#include "Common/assertion.h"
#include "Common/primitive_type.h"
#include "Core/Texture/SampleLocation.h"

#include <array>
#include <cstddef>
#include <algorithm>

namespace ph
{

template<std::size_t N>
class TNumericPixelTexture2D : public TPixelTexture2D<std::array<float64, N>>
{
public:
	explicit TNumericPixelTexture2D(const std::shared_ptr<PixelBuffer2D>& pixelBuffer);

	TNumericPixelTexture2D(
		const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
		EPixelTextureSampleMode               sampleMode,
		EPixelTextureWrapMode                 wrapMode);

	void sample(
		const SampleLocation&   sampleLocation, 
		std::array<float64, N>* out_value) const override;
};

// In-header Implementations:

template<std::size_t N>
inline TNumericPixelTexture2D<N>::TNumericPixelTexture2D(const std::shared_ptr<PixelBuffer2D>& pixelBuffer) :
	TPixelTexture2D<std::array<float64, N>>(pixelBuffer)
{}

template<std::size_t N>
inline TNumericPixelTexture2D<N>::TNumericPixelTexture2D(
	const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
	const EPixelTextureSampleMode         sampleMode,
	const EPixelTextureWrapMode           wrapMode) :

	TPixelTexture2D<std::array<float64, N>>(
		pixelBuffer,
		sampleMode,
		wrapMode)
{}

template<std::size_t N>
inline void TNumericPixelTexture2D<N>::sample(
	const SampleLocation&         sampleLocation,
	std::array<float64, N>* const out_value) const
{
	PH_ASSERT(out_value);

	const pixel_buffer::TPixel<float64> sampledPixel = samplePixelBuffer(sampleLocation.uv());

	// Number of sampled values is limited by either `N` or the dimension of sampled pixel
	const auto minBufferSize = std::min(N, sampledPixel.numValues());

	out_value->fill(0);
	for(std::size_t i = 0; i < minBufferSize; ++i)
	{
		(*out_value)[i] = sampledPixel[i];
	}
}

}// end namespace ph
