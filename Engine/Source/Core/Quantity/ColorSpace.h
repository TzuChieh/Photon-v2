#pragma once

#include "Math/TVector3.h"
#include "Common/assertion.h"

#include <cmath>

namespace ph
{

class ColorSpace final
{
public:
	static inline Vector3R sRgbToLinearRgb(const Vector3R& sRgb)
	{
		PH_ASSERT(sRgb.x >= 0.0_r && sRgb.x <= 1.0_r);
		PH_ASSERT(sRgb.y >= 0.0_r && sRgb.y <= 1.0_r);
		PH_ASSERT(sRgb.z >= 0.0_r && sRgb.z <= 1.0_r);

		return Vector3R(sRgbInverseGammaCorrect(sRgb.x),
		                sRgbInverseGammaCorrect(sRgb.y),
		                sRgbInverseGammaCorrect(sRgb.z));
	}

	static inline Vector3R linearRgbToSrgb(const Vector3R& linearRgb)
	{
		PH_ASSERT(linearRgb.x >= 0.0_r && linearRgb.x <= 1.0_r);
		PH_ASSERT(linearRgb.y >= 0.0_r && linearRgb.y <= 1.0_r);
		PH_ASSERT(linearRgb.z >= 0.0_r && linearRgb.z <= 1.0_r);

		return Vector3R(sRgbForwardGammaCorrect(linearRgb.x),
		                sRgbForwardGammaCorrect(linearRgb.y),
		                sRgbForwardGammaCorrect(linearRgb.z));
	}

private:
	static inline real sRgbForwardGammaCorrect(const real colorComponent)
	{
		if(colorComponent <= 0.0031308_r)
		{
			return 12.92_r * colorComponent;
		}

		return 1.055_r * std::pow(colorComponent, 1.0_r / 2.4_r) - 0.055_r;
	}

	static inline real sRgbInverseGammaCorrect(const real colorComponent)
	{
		if(colorComponent <= 0.04045_r)
		{
			return colorComponent / 12.92_r;
		}

		return std::pow((colorComponent + 0.055_r) / 1.055_r, 2.4_r);
	}
};

}// end namespace ph