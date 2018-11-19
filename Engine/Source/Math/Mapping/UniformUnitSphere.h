#pragma once

#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Math/constant.h"
#include "Common/assertion.h"

#include <cmath>
#include <algorithm>

namespace ph
{

class UniformUnitSphere
{
public:
	static Vector3R map(const Vector2R& seed);
	static Vector3R map(const Vector2R& seed, real* out_pdf);
};

// In-header Implementations:

inline Vector3R UniformUnitSphere::map(const Vector2R& seed)
{
	PH_ASSERT_LE(0.0_r, seed.x); PH_ASSERT_LE(seed.x, 1.0_r);
	PH_ASSERT_LE(0.0_r, seed.y); PH_ASSERT_LE(seed.y, 1.0_r);

	const real y   = 2.0_r * (seed.x - 0.5_r);
	const real phi = 2.0_r * PH_PI_REAL * seed.y;
	const real r   = std::sqrt(std::max(1.0_r - y * y, 0.0_r));

	return Vector3R(r * std::sin(phi), 
	                y, 
	                r * std::cos(phi));
}

inline Vector3R UniformUnitSphere::map(const Vector2R& seed, real* const out_pdf)
{
	// PDF is 1/(4*pi)
	PH_ASSERT(out_pdf);
	*out_pdf = PH_RECI_4_PI_REAL;

	return map(seed);
}

}// end namespace ph