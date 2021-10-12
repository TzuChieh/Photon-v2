#pragma once

#include "Core/Texture/Pixel/TNumericPixelTexture2D.h"
#include "Core/Texture/Pixel/pixel_texture_basics.h"

#include <cstddef>

namespace ph
{

class ScalarPixelTexture2D : public TNumericPixelTexture2D<1>
{
public:
	ScalarPixelTexture2D(
		const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
		pixel_texture::EPixelLayout           alphaLayout);

	ScalarPixelTexture2D(
		const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
		std::size_t                           pixelElementIndex);

	ScalarPixelTexture2D(
		const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
		pixel_texture::EPixelLayout           alphaLayout,
		pixel_texture::ESampleMode            sampleMode,
		pixel_texture::EWrapMode              wrapMode);

	ScalarPixelTexture2D(
		const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
		std::size_t                           pixelElementIndex,
		pixel_texture::ESampleMode            sampleMode,
		pixel_texture::EWrapMode              wrapMode);
};

}// end namespace ph
