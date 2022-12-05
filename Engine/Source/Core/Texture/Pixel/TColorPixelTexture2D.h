#pragma once

#include "Core/Texture/Pixel/TPixelTexture2D.h"
#include "Common/assertion.h"
#include "Common/primitive_type.h"
#include "Core/Texture/SampleLocation.h"
#include "Math/Color/color_enums.h"
#include "Math/Color/color_spaces.h"
#include "Math/Color/Spectrum.h"
#include "Core/Texture/Pixel/pixel_texture_basics.h"

#include <array>
#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include <format>

namespace ph
{

template<math::EColorSpace COLOR_SPACE>
class TColorPixelTexture2D : public TPixelTexture2D<math::Spectrum>
{
	static_assert(math::TColorSpaceDef<COLOR_SPACE>::isTristimulus(),
		"TColorPixelTexture2D Supports only tristimulus color space.");

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

	TPixelTexture2D<math::Spectrum>(
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

	PH_ASSERT_LE(pixel_texture::num_pixel_elements(m_colorLayout), getPixelBuffer()->numPixelElements());

	math::TTristimulusValues<float64> color{};
	switch(m_colorLayout)
	{
	case pixel_texture::EPixelLayout::R:
		color[0] = sampledPixel[0];
		break;

	case pixel_texture::EPixelLayout::G:
		color[1] = sampledPixel[0];
		break;

	case pixel_texture::EPixelLayout::B:
		color[2] = sampledPixel[0];
		break;

	case pixel_texture::EPixelLayout::Monochromatic:
		color.fill(sampledPixel[0]);
		break;

	case pixel_texture::EPixelLayout::RG:
		color[0] = sampledPixel[0];
		color[1] = sampledPixel[1];
		break;

	case pixel_texture::EPixelLayout::RGB:
	case pixel_texture::EPixelLayout::RGBA:
		color[0] = sampledPixel[0];
		color[1] = sampledPixel[1];
		color[2] = sampledPixel[2];
		break;

	case pixel_texture::EPixelLayout::BGR:
	case pixel_texture::EPixelLayout::BGRA:
		color[0] = sampledPixel[2];
		color[1] = sampledPixel[1];
		color[2] = sampledPixel[0];
		break;

	case pixel_texture::EPixelLayout::ARGB:
		color[0] = sampledPixel[1];
		color[1] = sampledPixel[2];
		color[2] = sampledPixel[3];
		break;

	case pixel_texture::EPixelLayout::ABGR:
		color[0] = sampledPixel[3];
		color[1] = sampledPixel[2];
		color[2] = sampledPixel[1];
		break;

	default:
		// The layout does not contain color
		PH_ASSERT_UNREACHABLE_SECTION();
		break;
	}

	const math::TristimulusValues castedColor = {
		static_cast<math::ColorValue>(color[0]),
		static_cast<math::ColorValue>(color[1]),
		static_cast<math::ColorValue>(color[2])};

	out_value->setTransformed<COLOR_SPACE>(castedColor, sampleLocation.expectedUsage());
}

}// end namespace ph
