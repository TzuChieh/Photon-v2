#pragma once

#include "Math/TVector3.h"
#include "Common/assertion.h"

#include <cmath>

namespace ph
{

class ColorSpace final
{
public:
	static inline Vector3R sRGB_to_linear_sRGB(const Vector3R& color)
	{
		return Vector3R(sRGB_inverseGammaCorrect(color.x),
		                sRGB_inverseGammaCorrect(color.y),
		                sRGB_inverseGammaCorrect(color.z));
	}

	static inline Vector3R linear_sRGB_to_sRGB(const Vector3R& color)
	{
		return Vector3R(sRGB_forwardGammaCorrect(color.x),
		                sRGB_forwardGammaCorrect(color.y),
		                sRGB_forwardGammaCorrect(color.z));
	}

	// CIE-XYZ color represented here is with a reference white point of D65

	static inline Vector3R CIE_XYZ_to_linear_sRGB(const Vector3R& color)
	{
		return Vector3R( 3.2404542_r * color.x - 1.5371385_r * color.y - 0.4985314_r * color.z,
		                -0.9692660_r * color.x + 1.8760108_r * color.y + 0.0415560_r * color.z,
		                 0.0556434_r * color.x - 0.2040259_r * color.y + 1.0572252_r * color.z);
	}

	static inline Vector3R linear_sRGB_to_CIE_XYZ(const Vector3R& color)
	{
		PH_ASSERT(color.x >= 0.0_r && color.x <= 1.0_r &&
		          color.y >= 0.0_r && color.y <= 1.0_r &&
		          color.z >= 0.0_r && color.z <= 1.0_r);

		return Vector3R(0.4124564_r * color.x + 0.3575761_r * color.y + 0.1804375_r * color.z, 
		                0.2126729_r * color.x + 0.7151522_r * color.y + 0.0721750_r * color.z,
		                0.0193339_r * color.x + 0.1191920_r * color.y + 0.9503041_r * color.z);
	}

private:
	static inline real sRGB_forwardGammaCorrect(const real colorComponent)
	{
		PH_ASSERT(colorComponent >= 0.0_r && colorComponent <= 1.0_r);

		if(colorComponent <= 0.0031308_r)
		{
			return 12.92_r * colorComponent;
		}

		return 1.055_r * std::pow(colorComponent, 1.0_r / 2.4_r) - 0.055_r;
	}

	static inline real sRGB_inverseGammaCorrect(const real colorComponent)
	{
		PH_ASSERT(colorComponent >= 0.0_r && colorComponent <= 1.0_r);

		if(colorComponent <= 0.04045_r)
		{
			return colorComponent / 12.92_r;
		}

		return std::pow((colorComponent + 0.055_r) / 1.055_r, 2.4_r);
	}
};

}// end namespace ph