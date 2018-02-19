#pragma once

#include "Math/TVector3.h"
#include "Common/assertion.h"

#include <cmath>

namespace ph
{

class ColorSpace final
{
public:
	static inline Vector3R srgbToLinearSrgb(const Vector3R& srgb)
	{
		return Vector3R(srgbInverseGammaCorrect(srgb.x),
		                srgbInverseGammaCorrect(srgb.y),
		                srgbInverseGammaCorrect(srgb.z));
	}

	static inline Vector3R linearSrgbToSrgb(const Vector3R& linearRgb)
	{
		return Vector3R(srgbForwardGammaCorrect(linearRgb.x),
		                srgbForwardGammaCorrect(linearRgb.y),
		                srgbForwardGammaCorrect(linearRgb.z));
	}

private:
	static inline real srgbForwardGammaCorrect(const real colorComponent)
	{
		PH_ASSERT(colorComponent >= 0.0_r && colorComponent <= 1.0_r);

		if(colorComponent <= 0.0031308_r)
		{
			return 12.92_r * colorComponent;
		}

		return 1.055_r * std::pow(colorComponent, 1.0_r / 2.4_r) - 0.055_r;
	}

	static inline real srgbInverseGammaCorrect(const real colorComponent)
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