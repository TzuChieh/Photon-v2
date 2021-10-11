#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector2.h"

#include <array>

namespace ph
{

enum class EPixelTextureWrapMode
{
	Repeat,
	ClampToEdge
};

enum class EPixelTextureSampleMode
{
	Nearest,
	Bilinear,
	Trilinear
};

/*! @brief Transform (u, v) coordinates to [0, 1] according to wrap mode.
*/
math::Vector2D normalize_texture_uv(const math::Vector2D& inputUV, const EPixelTextureWrapMode wrapMode)
{
	math::Vector2D outputUV{};
	switch(wrapMode)
	{
	case EPixelTextureWrapMode::Repeat:
	{
		const float64 fu = math::fractional_part(inputUV.u());
		const float64 fv = math::fractional_part(inputUV.v());
		outputUV.u() = fu >= 0.0 ? fu : fu + 1.0;
		outputUV.v() = fv >= 0.0 ? fv : fv + 1.0;
		break;
	}
	case EPixelTextureWrapMode::ClampToEdge:
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

}// end namespace ph
