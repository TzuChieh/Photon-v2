#pragma once

#include "Core/Quantity/ColorSpace.h"
#include "Common/assertion.h"
#include "Core/Quantity/SpectralStrength/TSampledSpectralStrength.h"

#include <type_traits>

namespace ph
{

template<typename Hint, typename>
inline Vector3R ColorSpace::SPD_to_CIE_XYZ(const SampledSpectralStrength& spd)
{
	PH_ASSERT(isInitialized());

	if constexpr(std::is_same_v<Hint, SourceHint::ILLUMINANT>)
	{
		return SPD_to_CIE_XYZ_D65(spd);
	}
	
	if constexpr(std::is_same_v<Hint, SourceHint::REFLECTANCE>)
	{
		return SPD_to_CIE_XYZ_E(spd);
	}

	// Assuming E white point based SPD for other cases.
	//
	return SPD_to_CIE_XYZ_E(spd);
}

template<typename Hint, typename>
inline Vector3R ColorSpace::SPD_to_linear_sRGB(const SampledSpectralStrength& spd)
{
	const Vector3R& cieXyz = SPD_to_CIE_XYZ<Hint>(spd);

	if constexpr(std::is_same_v<Hint, SourceHint::ILLUMINANT>)
	{
		return CIE_XYZ_D65_to_linear_sRGB(cieXyz);
	}

	if constexpr(std::is_same_v<Hint, SourceHint::REFLECTANCE>)
	{
		return CIE_XYZ_E_to_linear_sRGB(cieXyz);
	}

	// Assuming E white point based SPD for other cases.
	//
	return CIE_XYZ_E_to_linear_sRGB(SPD_to_CIE_XYZ_E(spd));
}

template<typename Hint, typename>
inline Vector3R ColorSpace::SPD_to_sRGB(const SampledSpectralStrength& spd)
{
	return linear_sRGB_to_sRGB(SPD_to_linear_sRGB<Hint>(spd));
}

template<typename Hint, typename>
inline void ColorSpace::linear_sRGB_to_SPD(
	const Vector3R&                color, 
	SampledSpectralStrength* const out_spd)
{
	PH_ASSERT(isInitialized());
	PH_ASSERT(out_spd != nullptr);

	const real r = color.x;
	const real g = color.y;
	const real b = color.z;

	out_spd->setValues(0);

	// The following steps mix in primary colors only as needed. Also, 
	// (r, g, b) = (1, 1, 1) will be mapped to a constant SPD with 
	// magnitudes = 1.

	// when R is minimum
	if(r <= g && r <= b)
	{
		out_spd->addLocal(SPD_Smits_E_white * r);
		if(g <= b)
		{
			out_spd->addLocal(SPD_Smits_E_cyan * (g - r));
			out_spd->addLocal(SPD_Smits_E_blue * (b - g));
		}
		else
		{
			out_spd->addLocal(SPD_Smits_E_cyan * (b - r));
			out_spd->addLocal(SPD_Smits_E_green * (g - b));
		}
	}
	// when G is minimum
	else if(g <= r && g <= b)
	{
		out_spd->addLocal(SPD_Smits_E_white * g);
		if(r <= b)
		{
			out_spd->addLocal(SPD_Smits_E_magenta * (r - g));
			out_spd->addLocal(SPD_Smits_E_blue * (b - r));
		}
		else
		{
			out_spd->addLocal(SPD_Smits_E_magenta * (b - g));
			out_spd->addLocal(SPD_Smits_E_red * (r - b));
		}
	}
	// when B is minimum
	else
	{
		out_spd->addLocal(SPD_Smits_E_white * b);
		if(r <= g)
		{
			out_spd->addLocal(SPD_Smits_E_yellow * (r - b));
			out_spd->addLocal(SPD_Smits_E_green * (g - r));
		}
		else
		{
			out_spd->addLocal(SPD_Smits_E_yellow * (g - b));
			out_spd->addLocal(SPD_Smits_E_red * (r - g));
		}
	}
	
	// For illuminants, scale its SPD so that constant SPDs matches D65.
	//
	if constexpr(std::is_same_v<Hint, SourceHint::ILLUMINANT>)
	{
		out_spd->mulLocal(SPD_D65);
	}

	// For reflectances, make sure energy conservation requirements are met.
	//
	if constexpr(std::is_same_v<Hint, SourceHint::REFLECTANCE>)
	{
		out_spd->clampLocal(0.0_r, 1.0_r);
	}
}

template<typename Hint, typename>
inline void ColorSpace::sRGB_to_SPD(const Vector3R& color, SampledSpectralStrength* const out_spd)
{
	linear_sRGB_to_SPD<Hint>(sRGB_to_linear_sRGB(color), out_spd);
}

}// end namespace ph