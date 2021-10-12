#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector2.h"

#include <array>
#include <cstddef>

namespace ph
{

namespace pixel_texture
{

enum class EWrapMode
{
	Repeat = 0,
	ClampToEdge
};

enum class ESampleMode
{
	Nearest = 0,
	Bilinear,
	Trilinear
};

enum class EPixelLayout
{
	PL_RGBA = 0,
	PL_R,
	PL_G,
	PL_B,
	PL_Monochromatic,
	PL_A,
	PL_RG,
	PL_RGB,
	PL_BGR,
	PL_ARGB,
	PL_ABGR,
	PL_BGRA
};

/*! @brief Transform (u, v) coordinates to [0, 1] according to wrap mode.
*/
inline math::Vector2D normalize_uv(const math::Vector2D& inputUV, const EWrapMode wrapMode)
{
	math::Vector2D outputUV{};
	switch(wrapMode)
	{
	case EWrapMode::Repeat:
	{
		const float64 fu = math::fractional_part(inputUV.u());
		const float64 fv = math::fractional_part(inputUV.v());
		outputUV.u() = fu >= 0.0 ? fu : fu + 1.0;
		outputUV.v() = fv >= 0.0 ? fv : fv + 1.0;
		break;
	}
	case EWrapMode::ClampToEdge:
	{
		outputUV = inputUV.clamp(0.0, 1.0);
		break;
	}
	default:
		PH_ASSERT_UNREACHABLE_SECTION();
	}

	PH_ASSERT_IN_RANGE_INCLUSIVE(outputUV.u(), 0.0, 1.0);
	PH_ASSERT_IN_RANGE_INCLUSIVE(outputUV.v(), 0.0, 1.0);
	return outputUV;
}

inline std::size_t num_pixel_elements(const EPixelLayout layout)
{
	switch(layout)
	{
	case EPixelLayout::PL_R:
	case EPixelLayout::PL_G:
	case EPixelLayout::PL_B:
	case EPixelLayout::PL_Monochromatic:
	case EPixelLayout::PL_A:
		return 1;

	case EPixelLayout::PL_RG:
		return 2;

	case EPixelLayout::PL_RGB:
	case EPixelLayout::PL_BGR:
		return 3;

	case EPixelLayout::PL_RGBA:
	case EPixelLayout::PL_ARGB:
	case EPixelLayout::PL_ABGR:
	case EPixelLayout::PL_BGRA:
		return 4;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return 0;
	}
}

}// end namespace pixel_texture

}// end namespace ph
