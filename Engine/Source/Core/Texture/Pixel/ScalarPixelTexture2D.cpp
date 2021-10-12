#include "Core/Texture/Pixel/ScalarPixelTexture2D.h"

namespace ph
{

ScalarPixelTexture2D::ScalarPixelTexture2D(
	const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
	const pixel_texture::EPixelLayout     alphaLayout) :

	ScalarPixelTexture2D(
		pixelBuffer,
		pixel_texture::alpha_channel_index(alphaLayout))
{}

ScalarPixelTexture2D::ScalarPixelTexture2D(
	const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
	const std::size_t                     pixelElementIndex) :
	
	ScalarPixelTexture2D(
		pixelBuffer,
		pixelElementIndex,
		pixel_texture::ESampleMode::Bilinear,
		pixel_texture::EWrapMode::Repeat)
{}

ScalarPixelTexture2D::ScalarPixelTexture2D(
	const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
	const pixel_texture::EPixelLayout     alphaLayout,
	const pixel_texture::ESampleMode      sampleMode,
	const pixel_texture::EWrapMode        wrapMode) :

	ScalarPixelTexture2D(
		pixelBuffer,
		pixel_texture::alpha_channel_index(alphaLayout),
		sampleMode,
		wrapMode)
{}

ScalarPixelTexture2D::ScalarPixelTexture2D(
	const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
	const std::size_t                     pixelElementIndex,
	const pixel_texture::ESampleMode      sampleMode,
	const pixel_texture::EWrapMode        wrapMode) :

	TNumericPixelTexture2D<1>(
		pixelBuffer,
		sampleMode,
		wrapMode,
		pixelElementIndex)
{}

}// end namespace ph
