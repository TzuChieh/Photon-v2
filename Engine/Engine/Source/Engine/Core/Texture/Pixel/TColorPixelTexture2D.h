#pragma once

#include "Engine/Core/Texture/Pixel/TPixelTexture2D.h"
#include "Engine/Core/Texture/SampleLocation.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Math/Color/color_enums.h"
#include "Engine/Math/Color/color_spaces.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/Texture/Pixel/pixel_texture_basics.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <array>
#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include <format>

namespace ph
{

/*! @brief Pixel texture with explicit tristimulus or Raw component semantics.
@tparam COLOR_SPACE Source color space. `Unspecified` stores Raw values without color transforms.
Monochromatic Raw pixels fill every working color space component. Other Raw layouts require a
tristimulus working color space, where channels map directly to its components.
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

	if constexpr(
		COLOR_SPACE == math::EColorSpace::Unspecified &&
		!math::is_tristimulus(math::Spectrum::getColorSpace()))
	{
		if(m_colorLayout != pixel_texture::EPixelLayout::Monochromatic)
		{
			throw std::invalid_argument(
				"Raw non-monochromatic pixels require a tristimulus working color space");
		}
	}
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

	// Produce working color space values from the sampled components
	if constexpr(COLOR_SPACE != math::EColorSpace::Unspecified)
	{
		out_value->setTransformed<COLOR_SPACE>(castedColor, sampleLocation.expectedUsage());
	}
	// Store Raw values without color conversion
	else
	{
		if constexpr(math::is_tristimulus(math::Spectrum::getColorSpace()))
		{
			// Store Raw components directly
			out_value->setColorValues(castedColor);
		}
		else
		{
			// Fill every spectral component with the monochromatic Raw value.
			PH_ASSERT(m_colorLayout == pixel_texture::EPixelLayout::Monochromatic);
			out_value->setColorValues(castedColor[0]);
		}
	}
}

}// end namespace ph
