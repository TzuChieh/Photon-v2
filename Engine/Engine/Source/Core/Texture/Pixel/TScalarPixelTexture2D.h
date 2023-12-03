#pragma once

#include "Core/Texture/Pixel/TNumericPixelTexture2D.h"
#include "Core/Texture/Pixel/pixel_texture_basics.h"

#include <cstddef>

namespace ph
{

template<typename T>
class TScalarPixelTexture2D : public TNumericPixelTexture2D<T, 1>
{
public:
	TScalarPixelTexture2D(
		const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
		pixel_texture::EPixelLayout           alphaLayout);

	TScalarPixelTexture2D(
		const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
		std::size_t                           pixelElementIndex);

	TScalarPixelTexture2D(
		const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
		pixel_texture::EPixelLayout           alphaLayout,
		pixel_texture::ESampleMode            sampleMode,
		pixel_texture::EWrapMode              wrapModeS,
		pixel_texture::EWrapMode              wrapModeT);

	TScalarPixelTexture2D(
		const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
		std::size_t                           pixelElementIndex,
		pixel_texture::ESampleMode            sampleMode,
		pixel_texture::EWrapMode              wrapModeS,
		pixel_texture::EWrapMode              wrapModeT);

	T sampleScalar(const SampleLocation& sampleLocation) const;
};

// In-header Implementations:

template<typename T>
inline TScalarPixelTexture2D<T>::TScalarPixelTexture2D(
	const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
	const pixel_texture::EPixelLayout     alphaLayout) :

	TScalarPixelTexture2D(
		pixelBuffer,
		pixel_texture::alpha_channel_index(alphaLayout))
{}

template<typename T>
inline TScalarPixelTexture2D<T>::TScalarPixelTexture2D(
	const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
	const std::size_t                     pixelElementIndex) :
	
	TScalarPixelTexture2D(
		pixelBuffer,
		pixelElementIndex,
		pixel_texture::ESampleMode::Bilinear,
		pixel_texture::EWrapMode::Repeat,
		pixel_texture::EWrapMode::Repeat)
{}

template<typename T>
inline TScalarPixelTexture2D<T>::TScalarPixelTexture2D(
	const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
	const pixel_texture::EPixelLayout     alphaLayout,
	const pixel_texture::ESampleMode      sampleMode,
	const pixel_texture::EWrapMode        wrapModeS,
	const pixel_texture::EWrapMode        wrapModeT) :

	TScalarPixelTexture2D(
		pixelBuffer,
		pixel_texture::alpha_channel_index(alphaLayout),
		sampleMode,
		wrapModeS,
		wrapModeT)
{}

template<typename T>
inline TScalarPixelTexture2D<T>::TScalarPixelTexture2D(
	const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
	const std::size_t                     pixelElementIndex,
	const pixel_texture::ESampleMode      sampleMode,
	const pixel_texture::EWrapMode        wrapModeS,
	const pixel_texture::EWrapMode        wrapModeT) :

	TNumericPixelTexture2D<T, 1>(
		pixelBuffer,
		sampleMode,
		wrapModeS,
		wrapModeT,
		pixelElementIndex)
{}

template<typename T>
inline T TScalarPixelTexture2D<T>::sampleScalar(const SampleLocation& sampleLocation) const
{
	math::TArithmeticArray<T, 1> arrValue;
	this->sample(sampleLocation, &arrValue);

	return arrValue[0];
}

}// end namespace ph
