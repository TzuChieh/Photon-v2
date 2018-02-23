#pragma once

#include "Math/TVector3.h"
#include "Common/assertion.h"
#include "Common/config.h"
#include "Core/Quantity/SpectralStrength.h"

#include <cmath>
#include <cstddef>

namespace ph
{

class ColorSpace final
{
public:
	// This method must be called once before using any other method.
	//
	static void init();

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
	// (actually the standard sRGB based on)
	//
	// Reference: 
	// (1) http://www.color.org/sRGB.xalter
	// (2) http://www.ryanjuckett.com/programming/rgb-color-space-conversion/?start=2

	static inline Vector3R CIE_XYZ_D65_to_linear_sRGB(const Vector3R& color)
	{
		return Vector3R( 3.241030_r * color.x - 1.537410_r * color.y - 0.498620_r * color.z,
		                -0.969242_r * color.x + 1.875960_r * color.y + 0.041555_r * color.z,
		                 0.055632_r * color.x - 0.203979_r * color.y + 1.056980_r * color.z);
	}

	static inline Vector3R linear_sRGB_to_CIE_XYZ_D65(const Vector3R& color)
	{
		return Vector3R(0.412383_r * color.x + 0.357585_r * color.y + 0.180480_r * color.z,
		                0.212635_r * color.x + 0.715170_r * color.y + 0.072192_r * color.z,
		                0.019330_r * color.x + 0.119195_r * color.y + 0.950528_r * color.z);
	}


	static inline Vector3R SPD_to_CIE_XYZ_D65(const SampledSpectralStrength& spd)
	{
		PH_ASSERT(isInitialized());

		return Vector3R(kernel_X_D65.dot(spd), 
		                kernel_Y_D65.dot(spd), 
		                kernel_Z_D65.dot(spd));
	}

	static inline const SampledSpectralStrength& get_D65_SPD()
	{
		PH_ASSERT(isInitialized());

		return SPD_D65;
	}

private:
	static SampledSpectralStrength kernel_X_D65;
	static SampledSpectralStrength kernel_Y_D65;
	static SampledSpectralStrength kernel_Z_D65;

	static SampledSpectralStrength SPD_D65;

#ifdef PH_DEBUG
	static inline bool isInitialized(const bool toggle = false)
	{
		static bool hasInit = false;

		hasInit = toggle ? !hasInit : hasInit;
		return hasInit;
	}
#endif

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