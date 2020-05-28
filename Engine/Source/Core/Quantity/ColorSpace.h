#pragma once

#include "Math/TVector3.h"
#include "Common/assertion.h"
#include "Common/config.h"
#include "Core/Quantity/spectrum_fwd.h"

#include <cmath>
#include <cstddef>
#include <type_traits>

namespace ph
{

// These are hints that can be specified while converting data between 
// color spaces. Specifying a hint to a method may results in better 
// result depending on the implementation.
//
enum class ESourceHint
{
	RAW_DATA,
	ILLUMINANT,
	REFLECTANCE,
};

class ColorSpace final
{
public:
	// This method must be called once before using any other method.
	//
	static void init();

	static inline math::Vector3R sRGB_to_linear_sRGB(const math::Vector3R& color)
	{
		return math::Vector3R(
			sRGB_inverseGammaCorrect(color.x),
			sRGB_inverseGammaCorrect(color.y),
			sRGB_inverseGammaCorrect(color.z));
	}

	static inline math::Vector3R linear_sRGB_to_sRGB(const math::Vector3R& color)
	{
		return math::Vector3R(
			sRGB_forwardGammaCorrect(color.x),
			sRGB_forwardGammaCorrect(color.y),
			sRGB_forwardGammaCorrect(color.z));
	}

	// Transforming a CIE-XYZ color to linear sRGB color space.
	//
	// Reference: 
	// (1) http://www.ryanjuckett.com/programming/rgb-color-space-conversion/?start=2
	// (2) http://www.color.org/sRGB.xalter

	// Using D65 as reference white point (standard sRGB is based on D65).

	static inline math::Vector3R CIE_XYZ_D65_to_linear_sRGB(const math::Vector3R& color)
	{
		return math::Vector3R( 
			 3.241030_r * color.x - 1.537410_r * color.y - 0.498620_r * color.z,
			-0.969242_r * color.x + 1.875960_r * color.y + 0.041555_r * color.z,
			 0.055632_r * color.x - 0.203979_r * color.y + 1.056980_r * color.z);
	}

	static inline math::Vector3R linear_sRGB_to_CIE_XYZ_D65(const math::Vector3R& color)
	{
		return math::Vector3R(
			0.412383_r * color.x + 0.357585_r * color.y + 0.180480_r * color.z,
			0.212635_r * color.x + 0.715170_r * color.y + 0.072192_r * color.z,
			0.019330_r * color.x + 0.119195_r * color.y + 0.950528_r * color.z);
	}

	static inline math::Vector3R CIE_XYZ_D65_to_sRGB(const math::Vector3R& color)
	{
		return linear_sRGB_to_sRGB(CIE_XYZ_D65_to_linear_sRGB(color));
	}

	// Using E as reference white point (non-standard).

	static inline math::Vector3R CIE_XYZ_E_to_linear_sRGB(const math::Vector3R& color)
	{
		return math::Vector3R(
			 2.689666_r * color.x - 1.275867_r * color.y - 0.413795_r * color.z,
			-1.022106_r * color.x + 1.978283_r * color.y + 0.043821_r * color.z,
			 0.061225_r * color.x - 0.224491_r * color.y + 1.163269_r * color.z);
	}

	static inline math::Vector3R linear_sRGB_to_CIE_XYZ_E(const math::Vector3R& color)
	{
		return math::Vector3R(
			0.496919_r * color.x + 0.339090_r * color.y + 0.163989_r * color.z,
			0.256224_r * color.x + 0.678181_r * color.y + 0.065595_r * color.z,
			0.023293_r * color.x + 0.113030_r * color.y + 0.863674_r * color.z);
	}

	static inline math::Vector3R CIE_XYZ_E_to_sRGB(const math::Vector3R& color)
	{
		return linear_sRGB_to_sRGB(CIE_XYZ_E_to_linear_sRGB(color));
	}

	// Conversions from color to relative luminance normalized to 1 for a 
	// reference white.
	//
	// Reference: https://en.wikipedia.org/wiki/Relative_luminance

	static inline real linear_sRGB_D65_to_luminance(const math::Vector3R& linearSrgb)
	{
		return 0.212635_r * linearSrgb.x + 0.715170_r * linearSrgb.y + 0.072192_r * linearSrgb.z;
	}

	static inline real linear_sRGB_E_to_luminance(const math::Vector3R& linearSrgb)
	{
		return 0.256224_r * linearSrgb.x + 0.678181_r * linearSrgb.y + 0.065595_r * linearSrgb.z;
	}

	static inline real CIE_XYZ_D65_to_luminance(const math::Vector3R& cieXyzD65)
	{
		return cieXyzD65.y;
	}

	static inline real CIE_XYZ_E_to_luminance(const math::Vector3R& cieXyzE)
	{
		return cieXyzE.y;
	}

	static inline math::Vector3R XYZ_to_xyY(const math::Vector3R& XYZ)
	{
		const auto sumXYZ = XYZ.x + XYZ.y + XYZ.z;
		if(sumXYZ != 0.0_r)
		{
			const auto rcpSumXYZ = 1.0_r / sumXYZ;
			return {XYZ.x * rcpSumXYZ, XYZ.y * rcpSumXYZ, XYZ.z * rcpSumXYZ};
		}
		else
		{
			return {0, 0, 0};
		}
	}

	static inline math::Vector3R xyY_to_XYZ(const math::Vector3R& xyY)
	{
		if(xyY.y != 0.0_r)
		{
			const auto rcp_y = 1.0_r / xyY.y;
			return {xyY.z * rcp_y * xyY.x, xyY.z, xyY.z * rcp_y * (1.0_r - xyY.x - xyY.y)};
		}
		else
		{
			return {0, 0, 0};
		}
	}

	template<ESourceHint HINT = ESourceHint::RAW_DATA>
	static math::Vector3R SPD_to_CIE_XYZ_D65(const SampledSpectrum& spd);

	template<ESourceHint HINT = ESourceHint::RAW_DATA>
	static math::Vector3R SPD_to_CIE_XYZ_E(const SampledSpectrum& spd);

	template<ESourceHint HINT = ESourceHint::RAW_DATA>
	static inline math::Vector3R SPD_to_CIE_XYZ(const SampledSpectrum& spd);

	template<ESourceHint HINT = ESourceHint::RAW_DATA>
	static inline math::Vector3R SPD_to_linear_sRGB(const SampledSpectrum& spd);

	template<ESourceHint HINT = ESourceHint::RAW_DATA>
	static inline math::Vector3R SPD_to_sRGB(const SampledSpectrum& spd);

	template<ESourceHint HINT = ESourceHint::RAW_DATA>
	static inline void linear_sRGB_to_SPD(const math::Vector3R& color, SampledSpectrum* out_spd);

	template<ESourceHint HINT = ESourceHint::RAW_DATA>
	static inline void sRGB_to_SPD(const math::Vector3R& color, SampledSpectrum* out_spd);

	static inline const SampledSpectrum& get_D65_SPD()
	{
		PH_ASSERT(isInitialized());

		return SPD_D65;
	}

	static inline const SampledSpectrum& get_E_SPD()
	{
		PH_ASSERT(isInitialized());

		return SPD_E;
	}

private:
#ifdef PH_DEBUG
	static inline bool isInitialized(const bool toggle = false)
	{
		static bool hasInit = false;

		hasInit = toggle ? !hasInit : hasInit;
		return hasInit;
	}
#endif

	static SampledSpectrum SPD_E;
	static SampledSpectrum SPD_D65;

	static SampledSpectrum kernel_X;
	static SampledSpectrum kernel_Y;
	static SampledSpectrum kernel_Z;
	static math::Vector3R kernel_XYZ_E_norm;
	static math::Vector3R kernel_XYZ_D65_norm;

	static SampledSpectrum SPD_Smits_E_white;
	static SampledSpectrum SPD_Smits_E_cyan;
	static SampledSpectrum SPD_Smits_E_magenta;
	static SampledSpectrum SPD_Smits_E_yellow;
	static SampledSpectrum SPD_Smits_E_red;
	static SampledSpectrum SPD_Smits_E_green;
	static SampledSpectrum SPD_Smits_E_blue;

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

#include "Core/Quantity/ColorSpace/ColorSpace.ipp"
