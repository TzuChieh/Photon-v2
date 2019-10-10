#pragma once

#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Math/constant.h"
#include "Common/assertion.h"

#include <cmath>

namespace ph::math
{

class CosLobeWeightedUnitHemisphere
{
public:
	explicit CosLobeWeightedUnitHemisphere(real exponent);

	Vector3R map(const Vector2R& seed) const;
	Vector3R map(const Vector2R& seed, real* out_pdf) const;

private:
	real m_exponent;
};

// In-header Implementations:

inline CosLobeWeightedUnitHemisphere::CosLobeWeightedUnitHemisphere(const real exponent) :
	m_exponent(exponent)
{}

inline Vector3R CosLobeWeightedUnitHemisphere::map(const Vector2R& seed) const
{
	PH_ASSERT_LE(0.0_r, seed.x); PH_ASSERT_LE(seed.x, 1.0_r);
	PH_ASSERT_LE(0.0_r, seed.y); PH_ASSERT_LE(seed.y, 1.0_r);

	const real phi      = constant::two_pi<real> * seed.x;
	const real cosTheta = std::pow(seed.y, 1.0_r / (m_exponent + 1.0_r));
	const real sinTheta = std::sqrt(1.0_r - cosTheta * cosTheta);

	return Vector3R(std::sin(phi) * sinTheta,
	                cosTheta,
	                std::cos(phi) * sinTheta);
}

inline Vector3R CosLobeWeightedUnitHemisphere::map(const Vector2R& seed, real* const out_pdf) const
{
	const Vector3R& mapped = map(seed);
	PH_ASSERT_GE(mapped.y, 0.0_r);

	// PDF is (exponent+1)/(2*pi)*cos(theta)^n
	// (note that cos(theta) is mapped.y)
	PH_ASSERT(out_pdf);
	*out_pdf = (m_exponent + 1) * constant::rcp_two_pi<real> * std::pow(mapped.y, m_exponent);

	return mapped;
}

}// end namespace ph::math
