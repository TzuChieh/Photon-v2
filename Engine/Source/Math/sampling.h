#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Math/TVector2.h"
#include "Math/constant.h"

#include <cmath>
#include <algorithm>

namespace ph
{

namespace sampling
{

namespace unit_hemisphere::uniform
{
	inline void gen(
		const real      value_0_1_a, 
		const real      value_0_1_b, 
		Vector3R* const out_sample)
	{
		PH_ASSERT(value_0_1_a >= 0.0_r && value_0_1_a <= 1.0_r);
		PH_ASSERT(value_0_1_b >= 0.0_r && value_0_1_b <= 1.0_r);
		PH_ASSERT(out_sample);

		const real phi     = 2.0_r * PH_PI_REAL * value_0_1_a;
		const real yValue  = value_0_1_b;
		const real yRadius = std::sqrt(1.0_r - yValue * yValue);

		out_sample->x = std::sin(phi) * yRadius;
		out_sample->y = yValue;
		out_sample->z = std::cos(phi) * yRadius;

		PH_ASSERT_MSG(out_sample->isFinite(), "\n"
			"vector = " + out_sample->toString() + "\n"
			"seed-a = " + std::to_string(value_0_1_a) + "\n"
			"seed-b = " + std::to_string(value_0_1_b) + "\n");
	}

	inline void gen(
		const real      value_0_1_a,
		const real      value_0_1_b,
		Vector3R* const out_sample,
		real* const     out_pdf)
	{
		gen(
			value_0_1_a,
			value_0_1_b,
			out_sample);

		PH_ASSERT(out_pdf);

		// sample PDF is 1/(2*pi)
		*out_pdf = 1.0_r / (2.0_r * PH_PI_REAL);
	}
}

namespace unit_hemisphere::cosine_theta_weighted
{
	inline void gen(
		const real      value_0_1_a, 
		const real      value_0_1_b, 
		Vector3R* const out_sample)
	{
		PH_ASSERT(value_0_1_a >= 0.0_r && value_0_1_a <= 1.0_r);
		PH_ASSERT(value_0_1_b >= 0.0_r && value_0_1_b <= 1.0_r);
		PH_ASSERT(out_sample);

		const real phi     = 2.0f * PH_PI_REAL * value_0_1_a;
		const real yValue  = std::sqrt(value_0_1_b);
		const real yRadius = std::sqrt(1.0f - yValue * yValue);// TODO: y*y is in fact valueB?

		out_sample->x = std::sin(phi) * yRadius;
		out_sample->y = yValue;
		out_sample->z = std::cos(phi) * yRadius;

		PH_ASSERT_MSG(out_sample->isFinite() && out_sample->length() > 0.9_r, "\n"
			"vector = " + out_sample->toString() + "\n"
			"seed-a = " + std::to_string(value_0_1_a) + "\n"
			"seed-b = " + std::to_string(value_0_1_b) + "\n");
	}

	inline void gen(
		const real      value_0_1_a,
		const real      value_0_1_b,
		Vector3R* const out_sample,
		real* const     out_pdf)
	{
		gen(
			value_0_1_a,
			value_0_1_b,
			out_sample);

		PH_ASSERT(out_pdf);
		PH_ASSERT_GE(out_sample->y, 0.0_r);

		// sample PDF is cos(theta)
		*out_pdf = out_sample->y;
	}
}

namespace unit_sphere::uniform
{
	inline void gen(
		const real      value_0_1_a, 
		const real      value_0_1_b, 
		Vector3R* const out_sample)
	{
		PH_ASSERT(value_0_1_a >= 0.0_r && value_0_1_a <= 1.0_r);
		PH_ASSERT(value_0_1_b >= 0.0_r && value_0_1_b <= 1.0_r);
		PH_ASSERT(out_sample);

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

		PH_ASSERT_MSG(out_sample->isFinite(), "\n"
			"vector = " + out_sample->toString() + "\n"
			"seed-a = " + std::to_string(value_0_1_a) + "\n"
			"seed-b = " + std::to_string(value_0_1_b) + "\n");
	}

	inline void gen(
		const real      value_0_1_a,
		const real      value_0_1_b,
		Vector3R* const out_sample,
		real* const     out_pdf)
	{
		gen(
			value_0_1_a,
			value_0_1_b,
			out_sample);

		PH_ASSERT(out_pdf);

		// sample PDF is 1/(4*pi)
		*out_pdf = PH_RECI_4_PI_REAL;
	}
}

namespace unit_disk::uniform
{
	inline void gen(
		const real      value_0_1_a,
		const real      value_0_1_b,
		Vector2R* const out_sample)
	{
		PH_ASSERT(value_0_1_a >= 0.0_r && value_0_1_a <= 1.0_r);
		PH_ASSERT(value_0_1_b >= 0.0_r && value_0_1_b <= 1.0_r);
		PH_ASSERT(out_sample);

		const real r   = std::sqrt(value_0_1_a);
		const real phi = 2.0_r * PH_PI_REAL * value_0_1_b;

		out_sample->x = r * std::cos(phi);
		out_sample->y = r * std::sin(phi);
	}

	inline void gen(
		const real      value_0_1_a,
		const real      value_0_1_b,
		Vector2R* const out_sample,
		real* const     out_pdf)
	{
		gen(
			value_0_1_a,
			value_0_1_b,
			out_sample);

		PH_ASSERT(out_pdf);

		// sample PDF is 1/pi
		*out_pdf = PH_RECI_PI_REAL;
	}
}

}// end namespace sampling

}// end namespace ph