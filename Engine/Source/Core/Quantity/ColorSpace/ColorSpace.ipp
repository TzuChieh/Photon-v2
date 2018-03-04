#pragma once

#include "Core/Quantity/ColorSpace.h"
#include "Common/assertion.h"

#include <type_traits>

namespace ph
{

template<typename Hint, typename>
static inline void ColorSpace::linear_sRGB_to_SPD(
	const Vector3R&                color, 
	SampledSpectralStrength* const out_spd)
{
	PH_ASSERT(isInitialized());
	PH_ASSERT(out_spd != nullptr);

	const real r = color.x;
	const real g = color.y;
	const real b = color.z;

	out_spd->setValues(0);

	// The following steps mix in primary colors only as needed.

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

	// TODO: ensure energy conservation?
}

template<typename Hint, typename>
void inline ColorSpace::sRGB_to_SPD(const Vector3R& color, SampledSpectralStrength* const out_spd)
{
	linear_sRGB_to_SPD<Hint>(sRGB_to_linear_sRGB(color), out_spd);
}

}// end namespace ph