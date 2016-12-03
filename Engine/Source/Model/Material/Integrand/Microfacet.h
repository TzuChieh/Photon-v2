#pragma once

#include "Common/primitive_type.h"
#include "Math/constant.h"
#include "Math/Vector3f.h"

namespace ph
{

class Microfacet final
{
public:
	// GGX (Trowbridge-Reitz) Normal Distribution Function
	inline static float32 normalDistributionGgxTrowbridgeReitz(const float32 NoH, const float32 roughness)
	{
		if(NoH <= 0.0f)
		{
			return 0.0f;
		}

		const float32 alpha = roughness * roughness;
		const float32 alpha2 = alpha * alpha;
		const float32 NoH2 = NoH * NoH;

		const float32 innerTerm = NoH2 * (alpha2 - 1.0f) + 1.0f;
		const float32 denominator = PI_FLOAT32 * innerTerm * innerTerm;

		return alpha2 / denominator;
	}

	// Smith's GGX Geometry Shadowing Function
	inline static float32 geometryShadowingGgxSmith(const float32 NoV, const float32 NoL, const float32 HoV, const float32 HoL, const float32 roughness)
	{
		if(HoL / NoL <= 0.0f || HoV / NoV <= 0.0f)
		{
			return 0.0f;
		}

		const float32 alpha = roughness * roughness;
		const float32 alpha2 = alpha * alpha;

		const float32 lightG = (2.0f * NoL) / (NoL + sqrt(alpha2 + (1.0f - alpha2) * NoL * NoL));
		const float32 viewG  = (2.0f * NoV) / (NoV + sqrt(alpha2 + (1.0f - alpha2) * NoV * NoV));

		return lightG * viewG;
	}

	// Schlick Approximated Fresnel Function
	inline static void fresnelSchlickApproximated(const float32 HoV, const Vector3f& f0, Vector3f* const out_F)
	{
		*out_F = f0.add(f0.complement().mulLocal(pow(1.0f - HoV, 5)));
	}
};

}// end namespace ph