#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Math/constant.h"

#include <cmath>

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

		out_sample->x = std::cos(phi) * yRadius;
		out_sample->y = yValue;
		out_sample->z = std::sin(phi) * yRadius;
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

		out_sample->x = std::cos(phi) * yRadius;
		out_sample->y = yValue;
		out_sample->z = std::sin(phi) * yRadius;
	}

}

}// end namespace sampling

}// end namespace ph