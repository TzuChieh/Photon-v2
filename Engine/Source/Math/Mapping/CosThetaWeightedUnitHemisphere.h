#pragma once

#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Math/constant.h"
#include "Common/assertion.h"

#include <cmath>

namespace ph::math
{

class CosThetaWeightedUnitHemisphere
{
public:
	static Vector3R map(const Vector2R& seed);
	static Vector3R map(const Vector2R& seed, real* out_pdf);
};

// In-header Implementations:

inline Vector3R CosThetaWeightedUnitHemisphere::map(const Vector2R& seed)
{
	PH_ASSERT_LE(0.0_r, seed.x); PH_ASSERT_LE(seed.x, 1.0_r);
	PH_ASSERT_LE(0.0_r, seed.y); PH_ASSERT_LE(seed.y, 1.0_r);

	const real phi     = constant::two_pi<real> * seed.x;
	const real yValue  = std::sqrt(seed.y);
	const real yRadius = std::sqrt(1.0f - yValue * yValue);// TODO: y*y is in fact valueB?

	return Vector3R(std::sin(phi) * yRadius, 
	                yValue, 
	                std::cos(phi) * yRadius);
}

inline Vector3R CosThetaWeightedUnitHemisphere::map(const Vector2R& seed, real* const out_pdf)
{
	const Vector3R& mapped = map(seed);
	PH_ASSERT_GE(mapped.y, 0.0_r);

	// PDF is cos(theta)/pi
	PH_ASSERT(out_pdf);
	*out_pdf = mapped.y * constant::rcp_pi<real>;

	return mapped;
}

}// end namespace ph::math
