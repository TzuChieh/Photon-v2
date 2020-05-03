#pragma once

#include "Core/Quantity/ColorSpace.h"
#include "Common/assertion.h"
#include "Core/Quantity/Spectrum.h"

#include <type_traits>

namespace ph
{

template<ESourceHint HINT>
inline math::Vector3R ColorSpace::SPD_to_CIE_XYZ_D65(const SampledSpectrum& spd)
{
	PH_ASSERT(isInitialized());

	if constexpr(HINT == ESourceHint::ILLUMINANT)
	{
		math::Vector3R xyz(
			kernel_X.dot(spd),
			kernel_Y.dot(spd),
			kernel_Z.dot(spd));

		xyz.mulLocal(kernel_XYZ_D65_norm);

		return xyz;
	}
	else if constexpr(HINT == ESourceHint::REFLECTANCE)
	{
		math::Vector3R xyz(
			kernel_X.dot(spd),
			kernel_Y.dot(spd),
			kernel_Z.dot(spd));

		xyz.clampLocal(0.0_r, 1.0_r);

		return xyz;
	}
	else
	{
		math::Vector3R xyz(
			kernel_X.dot(spd),
			kernel_Y.dot(spd),
			kernel_Z.dot(spd));

		return xyz;
	}
}

template<ESourceHint HINT>
inline math::Vector3R ColorSpace::SPD_to_CIE_XYZ_E(const SampledSpectrum& spd)
{
	PH_ASSERT(isInitialized());

	if constexpr(HINT == ESourceHint::ILLUMINANT)
	{
		math::Vector3R xyz(
			kernel_X.dot(spd),
			kernel_Y.dot(spd),
			kernel_Z.dot(spd));

		xyz.mulLocal(kernel_XYZ_E_norm);

		return xyz;
	}
	else if constexpr(HINT == ESourceHint::REFLECTANCE)
	{
		math::Vector3R xyz(
			kernel_X.dot(spd),
			kernel_Y.dot(spd),
			kernel_Z.dot(spd));

		xyz.clampLocal(0.0_r, 1.0_r);

		return xyz;
	}
	else
	{
		math::Vector3R xyz(
			kernel_X.dot(spd),
			kernel_Y.dot(spd),
			kernel_Z.dot(spd));

		return xyz;
	}
}

template<ESourceHint HINT>
inline math::Vector3R ColorSpace::SPD_to_CIE_XYZ(const SampledSpectrum& spd)
{
	PH_ASSERT(isInitialized());

	if constexpr(HINT == ESourceHint::ILLUMINANT)
	{
		return SPD_to_CIE_XYZ_D65<ESourceHint::ILLUMINANT>(spd);
	}
	else if constexpr(HINT == ESourceHint::REFLECTANCE)
	{
		return SPD_to_CIE_XYZ_E<ESourceHint::REFLECTANCE>(spd);
	}
	else
	{
		return SPD_to_CIE_XYZ_E<ESourceHint::RAW_DATA>(spd);
	}
}

template<ESourceHint HINT>
inline math::Vector3R ColorSpace::SPD_to_linear_sRGB(const SampledSpectrum& spd)
{
	if constexpr(HINT == ESourceHint::ILLUMINANT)
	{
		return CIE_XYZ_D65_to_linear_sRGB(SPD_to_CIE_XYZ_D65<ESourceHint::ILLUMINANT>(spd));
	}
	else if constexpr(HINT == ESourceHint::REFLECTANCE)
	{
		return CIE_XYZ_E_to_linear_sRGB(SPD_to_CIE_XYZ_E<ESourceHint::REFLECTANCE>(spd));
	}
	else
	{
		return CIE_XYZ_E_to_linear_sRGB(SPD_to_CIE_XYZ_E<ESourceHint::RAW_DATA>(spd));
	}
}

template<ESourceHint HINT>
inline math::Vector3R ColorSpace::SPD_to_sRGB(const SampledSpectrum& spd)
{
	return linear_sRGB_to_sRGB(SPD_to_linear_sRGB<HINT>(spd));
}

template<ESourceHint HINT>
inline void ColorSpace::linear_sRGB_to_SPD(
	const math::Vector3R&  color,
	SampledSpectrum* const out_spd)
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
	if constexpr(HINT == ESourceHint::ILLUMINANT)
	{
		out_spd->mulLocal(SPD_D65);
	}

	// For reflectances, make sure energy conservation requirements are met.
	//
	if constexpr(HINT == ESourceHint::REFLECTANCE)
	{
		out_spd->clampLocal(0.0_r, 1.0_r);
	}
}

template<ESourceHint HINT>
inline void ColorSpace::sRGB_to_SPD(const math::Vector3R& color, SampledSpectrum* const out_spd)
{
	linear_sRGB_to_SPD<HINT>(sRGB_to_linear_sRGB(color), out_spd);
}

}// end namespace ph
