#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Math/constant.h"

#include <cmath>
#include <algorithm>

namespace ph
{

namespace sampling
{

namespace unit_hemisphere::uniform
{
	// sample PDF is 1/(2*pi)
	//
	inline void gen(const real value_0_1_a, const real value_0_1_b, Vector3R* const out_sample)
	{
		PH_ASSERT(value_0_1_a >= 0.0_r && value_0_1_a <= 1.0_r);
		PH_ASSERT(value_0_1_b >= 0.0_r && value_0_1_b <= 1.0_r);
		PH_ASSERT(out_sample != nullptr);

		const real phi     = 2.0_r * PH_PI_REAL * value_0_1_a;
		const real yValue  = value_0_1_b;
		const real yRadius = std::sqrt(1.0_r - yValue * yValue);

		out_sample->x = std::sin(phi) * yRadius;
		out_sample->y = yValue;
		out_sample->z = std::cos(phi) * yRadius;
	}
}

namespace unit_hemisphere::cosine_theta_weighted
{
	// sample PDF is cos(theta)
	//
	inline void gen(const real value_0_1_a, const real value_0_1_b, Vector3R* const out_sample)
	{
		PH_ASSERT(value_0_1_a >= 0.0_r && value_0_1_a <= 1.0_r);
		PH_ASSERT(value_0_1_b >= 0.0_r && value_0_1_b <= 1.0_r);
		PH_ASSERT(out_sample != nullptr);

		const real phi     = 2.0f * PH_PI_REAL * value_0_1_a;
		const real yValue  = std::sqrt(value_0_1_b);
		const real yRadius = std::sqrt(1.0f - yValue * yValue);

		out_sample->x = std::sin(phi) * yRadius;
		out_sample->y = yValue;
		out_sample->z = std::cos(phi) * yRadius;
	}
}

namespace unit_sphere::uniform
{
	// sample PDF is 1/(4*pi)
	//
	inline void gen(const real value_0_1_a, const real value_0_1_b, Vector3R* const out_sample)
	{
		PH_ASSERT(value_0_1_a >= 0.0_r && value_0_1_a <= 1.0_r);
		PH_ASSERT(value_0_1_b >= 0.0_r && value_0_1_b <= 1.0_r);
		PH_ASSERT(out_sample != nullptr);

		/*const real r1       = value_0_1_a;
		const real r2       = value_0_1_b;
		const real sqrtTerm = std::sqrt(r2 * (1.0_r - r2));
		const real phiTerm  = 2.0_r * PH_PI_REAL * r1;
		out_sample->x = 2.0_r * std::sin(phiTerm) * sqrtTerm;
		out_sample->y = 1.0_r - 2.0_r * r2;
		out_sample->z = 2.0_r * std::cos(phiTerm) * sqrtTerm;
		out_sample->normalizeLocal();*/

		const real y   = 2.0_r * (value_0_1_a - 0.5_r);
		const real phi = 2.0_r * PH_PI_REAL * value_0_1_b;
		const real r   = std::sqrt(std::max(1.0_r - y * y, 0.0_r));

		out_sample->x = r * std::sin(phi);
		out_sample->y = y;
		out_sample->z = r * std::cos(phi);
		out_sample->normalizeLocal();
	}
}

}// end namespace sampling

}// end namespace ph