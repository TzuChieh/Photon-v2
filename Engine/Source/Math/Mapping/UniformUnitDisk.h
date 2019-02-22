#pragma once

#include "Math/TVector2.h"
#include "Math/constant.h"
#include "Common/assertion.h"

#include <cmath>

namespace ph
{

class UniformUnitDisk
{
public:
	static Vector2R map(const Vector2R& seed);
	static Vector2R map(const Vector2R& seed, real* out_pdf);
};

// In-header Implementations:

inline Vector2R UniformUnitDisk::map(const Vector2R& seed)
{
	PH_ASSERT_LE(0.0_r, seed.x); PH_ASSERT_LE(seed.x, 1.0_r);
	PH_ASSERT_LE(0.0_r, seed.y); PH_ASSERT_LE(seed.y, 1.0_r);

	const real r   = std::sqrt(seed.x);
	const real phi = constant::two_pi<real> * seed.y;

	return Vector2R(r * std::cos(phi), 
	                r * std::sin(phi));
}

inline Vector2R UniformUnitDisk::map(const Vector2R& seed, real* const out_pdf)
{
	// PDF is 1/pi
	PH_ASSERT(out_pdf);
	*out_pdf = constant::rcp_pi<real>;

	return map(seed);
}

}// end namespace ph