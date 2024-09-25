#pragma once

#include "Core/Texture/Pixel/TPixelTexture2D.h"
#include "Core/Texture/SampleLocation.h"
#include "Math/TVector3.h"
#include "Math/Color/color_enums.h"
#include "Math/Color/color_spaces.h"
#include "Math/Color/Spectrum.h"
#include "Core/Texture/Pixel/pixel_texture_basics.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/logging.h>

#include <array>
#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include <format>

namespace ph
{
	
/*!
@tparam COLOR_SPACE Source color space. The color space of the contained pixel data. If is
`math::EColorSpace::Unspecified`, then raw color data is used (without any space transformations).
Using `math::EColorSpace::Unspecified` when the engine is in spectral mode may fallback to
`math::EColorSpace::Linear_sRGB` if a direct conversion is impossible.
*/
template<math::EColorSpace COLOR_SPACE>
class TColorPixelTexture2D : public TPixelTexture2D<math::Spectrum>
{
	static_assert(
		math::TColorSpaceDef<COLOR_SPACE>::isTristimulus() ||
		COLOR_SPACE == math::EColorSpace::Unspecified,
		"`TColorPixelTexture2D` supports only tristimulus color space or raw color data.");

public:
	TColorPixelTexture2D(
		const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
		pixel_texture::EPixelLayout           colorLayout);

	TColorPixelTexture2D(
		const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
		pixel_texture::EPixelLayout           colorLayout,
		pixel_texture::ESampleMode            sampleMode,
		pixel_texture::EWrapMode              wrapModeS,
		pixel_texture::EWrapMode              wrapModeT);

	void sample(
		const SampleLocation& sampleLocation, 
		math::Spectrum*       out_value) const override;

private:
	pixel_texture::EPixelLayout m_colorLayout;
};

// In-header Implementations:

template<math::EColorSpace COLOR_SPACE>
inline TColorPixelTexture2D<COLOR_SPACE>::TColorPixelTexture2D(
	const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
	const pixel_texture::EPixelLayout     colorLayout) :

	TColorPixelTexture2D(
		pixelBuffer,
		colorLayout,
		pixel_texture::ESampleMode::Bilinear,
		pixel_texture::EWrapMode::Repeat,
		pixel_texture::EWrapMode::Repeat)
{}

template<math::EColorSpace COLOR_SPACE>
inline TColorPixelTexture2D<COLOR_SPACE>::TColorPixelTexture2D(
	const std::shared_ptr<PixelBuffer2D>& pixelBuffer,
	const pixel_texture::EPixelLayout     colorLayout,
	const pixel_texture::ESampleMode      sampleMode,
	const pixel_texture::EWrapMode        wrapModeS,
	const pixel_texture::EWrapMode        wrapModeT) :

	TPixelTexture2D<math::Spectrum>(
		pixelBuffer,
		sampleMode,
		wrapModeS,
		wrapModeT),

	m_colorLayout(colorLayout)
{
	const auto layoutSize = pixel_texture::num_pixel_elements(m_colorLayout);
	const auto pixelSize  = getPixelBuffer()->numPixelElements();
	if(layoutSize > pixelSize)
	{
		throw std::invalid_argument(std::format(
			"Pixel layout with {} pixel elements does not match a pixel buffer with {} pixel elements",
			layoutSize, pixelSize));
	}

#if PH_DEBUG
	if(COLOR_SPACE == math::EColorSpace::Unspecified && math::Spectrum::NUM_VALUES != 3)
	{
		PH_DEFAULT_DEBUG_LOG(
			"`TColorPixelTexture2D` will fallback to treating values as linear sRGB (reason: direct "
			"value conversion cannot be performed).");
	}
#endif
}

template<math::EColorSpace COLOR_SPACE>
inline void TColorPixelTexture2D<COLOR_SPACE>::sample(
	const SampleLocation& sampleLocation,
	math::Spectrum* const out_value) const
{
	PH_ASSERT(out_value);

	// FIXME: vec cast
	const pixel_buffer::TPixel<float64> sampledPixel = samplePixelBuffer(math::Vector2D(sampleLocation.uv()));

	// Get tristimulus color according to pixel buffer layout. Alpha is ignored for this texture.
	// TODO: we can also add a premultiplied alpha mode

	PH_ASSERT_LE(pixel_texture::num_pixel_elements(m_colorLayout), getPixelBuffer()->numPixelElements());

	math::TVector3<float64> color(0);
	switch(m_colorLayout)
	{
	case pixel_texture::EPixelLayout::R:
		color.r() = sampledPixel[0];
		break;

	case pixel_texture::EPixelLayout::G:
		color.g() = sampledPixel[0];
		break;

	case pixel_texture::EPixelLayout::B:
		color.b() = sampledPixel[0];
		break;

	// FIXME: monochromatic may not mean same value on all components -> HSV?
	case pixel_texture::EPixelLayout::Monochromatic:
		color.set(sampledPixel[0]);
		break;

	case pixel_texture::EPixelLayout::RG:
		color.r() = sampledPixel[0];
		color.g() = sampledPixel[1];
		break;

	case pixel_texture::EPixelLayout::RGB:
	case pixel_texture::EPixelLayout::RGBA:
		color.r() = sampledPixel[0];
		color.g() = sampledPixel[1];
		color.b() = sampledPixel[2];
		break;

	case pixel_texture::EPixelLayout::BGR:
	case pixel_texture::EPixelLayout::BGRA:
		color.r() = sampledPixel[2];
		color.g() = sampledPixel[1];
		color.b() = sampledPixel[0];
		break;

	case pixel_texture::EPixelLayout::ARGB:
		color.r() = sampledPixel[1];
		color.g() = sampledPixel[2];
		color.b() = sampledPixel[3];
		break;

	case pixel_texture::EPixelLayout::ABGR:
		color.r() = sampledPixel[3];
		color.g() = sampledPixel[2];
		color.b() = sampledPixel[1];
		break;

	default:
		// The layout does not contain color
		PH_ASSERT_UNREACHABLE_SECTION();
		break;
	}

	const math::TristimulusValues castedColor = {
		static_cast<math::ColorValue>(color.r()),
		static_cast<math::ColorValue>(color.g()),
		static_cast<math::ColorValue>(color.b())};

	// Transform color values to the engine color space
	if constexpr(COLOR_SPACE != math::EColorSpace::Unspecified)
	{
		out_value->setTransformed<COLOR_SPACE>(castedColor, sampleLocation.expectedUsage());
	}
	// This is raw color values
	else
	{
		// No transform needed when direct conversion is possible
		if constexpr(math::Spectrum::NUM_VALUES == 3)
		{
			out_value->setColorValues(castedColor);
		}
		// Best guess: raw data is in linear sRGB (as a fallback)
		else
		{
			out_value->setLinearSRGB(castedColor, sampleLocation.expectedUsage());
		}
	}
}

}// end namespace ph
