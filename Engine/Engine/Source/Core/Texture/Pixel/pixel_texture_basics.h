#pragma once

#include "Math/TVector2.h"
#include "Utility/utility.h"
#include "Math/math.h"

#include <Common/primitive_type.h>

#include <array>
#include <cstddef>
#include <stdexcept>
#include <format>

namespace ph
{

namespace pixel_texture
{

enum class EWrapMode
{
	Repeat = 0,
	ClampToEdge,
	FlippedClampToEdge,
};

enum class ESampleMode
{
	Nearest = 0,
	Bilinear,
	Trilinear
};

/*! @brief Pixel layout of pixel texture.
Represent the ordering of pixel components with respect to a color space. The characters RGB 
does not mean the pixel is in RGB color space. It simply represents pixel components, 
using R, G, B as placeholders.
*/
enum class EPixelLayout
{
	RGBA = 0,
	R,
	G,
	B,
	Monochromatic,
	A,
	RG,
	RGB,
	BGR,
	ARGB,
	ABGR,
	BGRA
};

/*! @brief Transform (u, v) coordinates to (s, t) in [0, 1] according to wrap mode.
This overload operates on single channel only. The transformation will preserve texel density 
as it originally is (e.g, no scaling effect).
*/
inline float64 uv_to_st_scalar(const float64 inputUV, const EWrapMode wrapMode)
{
	float64 outputST = 0.0;
	switch(wrapMode)
	{
	case EWrapMode::Repeat:
	{
		const float64 st = math::fractional_part(inputUV);
		outputST = st >= 0.0 ? st : st + 1.0;
		break;
	}

	case EWrapMode::ClampToEdge:
	{
		outputST = math::clamp(inputUV, 0.0, 1.0);
		break;
	}

	case EWrapMode::FlippedClampToEdge:
	{
		const float64 flippedST = inputUV * -1.0 + 1.0;
		outputST = math::clamp(flippedST, 0.0, 1.0);
		break;
	}

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
	}

	PH_ASSERT_IN_RANGE_INCLUSIVE(outputST, 0.0, 1.0);
	return outputST;
}

/*! @brief Transform (u, v) coordinates to (s, t) in [0, 1] according to wrap mode.
The transformation will preserve texel density as it originally is (e.g, no scaling effect).
*/
inline math::Vector2D uv_to_st(const math::Vector2D& inputUV, const EWrapMode wrapModeS, const EWrapMode wrapModeT)
{
	return
	{
		uv_to_st_scalar(inputUV.u(), wrapModeS),
		uv_to_st_scalar(inputUV.v(), wrapModeT)
	};
}

inline std::size_t num_pixel_elements(const EPixelLayout layout)
{
	switch(layout)
	{
	case EPixelLayout::R:
	case EPixelLayout::G:
	case EPixelLayout::B:
	case EPixelLayout::Monochromatic:
	case EPixelLayout::A:
		return 1;

	case EPixelLayout::RG:
		return 2;

	case EPixelLayout::RGB:
	case EPixelLayout::BGR:
		return 3;

	case EPixelLayout::RGBA:
	case EPixelLayout::ARGB:
	case EPixelLayout::ABGR:
	case EPixelLayout::BGRA:
		return 4;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return 0;
	}
}

inline std::size_t alpha_channel_index(const EPixelLayout layout)
{
	switch(layout)
	{
	case EPixelLayout::A:
	case EPixelLayout::ARGB:
	case EPixelLayout::ABGR:
		return 0;

	case EPixelLayout::RGBA:
	case EPixelLayout::BGRA:
		return 3;

	default:
		throw std::invalid_argument(std::format(
			"Pixel layout does not contain alpha channel: {}", enum_to_string(layout)));
	}
}

}// end namespace pixel_texture

}// end namespace ph
