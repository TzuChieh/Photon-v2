#pragma once

#include "Common/primitive_type.h"
#include "Math/constant.h"
#include "Math/TVector3.h"

#include <iostream>

namespace ph
{

class Microfacet final
{
public:
	// GGX (Trowbridge-Reitz) Normal Distribution Function
	inline static real normalDistributionGgxTrowbridgeReitz(const real NoH, const real alpha)
	{
		if(NoH <= 0.0_r)
		{
			return 0.0_r;
		}

		const real alpha2 = alpha * alpha;
		const real NoH2 = NoH * NoH;

		const real innerTerm = NoH2 * (alpha2 - 1.0_r) + 1.0_r;
		const real denominator = PI_REAL * innerTerm * innerTerm;

		return alpha2 / denominator;
	}

	// Smith's GGX Geometry Shadowing Function (H is expected to be on the hemisphere of N)
	inline static real geometryShadowingGgxSmith(const real NoV, const real NoL, const real HoV, const real HoL, const real alpha)
	{
		// The back surface of the microsurface is never visible from directions on the front side 
		// of the macrosurface and viceversa (sidedness agreement)
		if(HoL * NoL <= 0.0_r || HoV * NoV <= 0.0_r)
		{
			return 0.0_r;
		}

		const real alpha2 = alpha * alpha;

		const real lightG = 2.0_r / (1.0_r + sqrt(alpha2 * (1.0_r / (NoL*NoL) - 1.0_r) + 1.0_r));
		const real viewG  = 2.0_r / (1.0_r + sqrt(alpha2 * (1.0_r / (NoV*NoV) - 1.0_r) + 1.0_r));

		return lightG * viewG;
	}

	// Schlick Approximated Fresnel Function
	inline static void fresnelSchlickApproximated(const real HoV, const Vector3R& f0, Vector3R* const out_F)
	{
		*out_F = f0.add(f0.complement().mulLocal(pow(1.0_r - HoV, 5)));
	}
};

}// end namespace ph