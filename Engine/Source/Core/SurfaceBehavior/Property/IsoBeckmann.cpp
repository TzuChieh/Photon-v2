#include "Core/SurfaceBehavior/Property/IsoBeckmann.h"
#include "Common/assertion.h"
#include "Math/constant.h"
#include "Math/math.h"
#include "Math/TOrthonormalBasis3.h"

#include <cmath>
#include <limits>
#include <string>

namespace ph
{

IsoBeckmann::IsoBeckmann(const real alpha) :
	Microfacet()
{
	// TODO: test and see if a smaller alpha is possible
	m_alpha = alpha > 0.001_r ? alpha : 0.001_r;
}

// Beckmann Normal Distribution Function
real IsoBeckmann::distribution(
	const SurfaceHit&     X,
	const math::Vector3R& N,
	const math::Vector3R& H) const
{
	real NoH = N.dot(H);
	if(NoH <= 0.0_r)
	{
		return 0.0_r;
	}
	NoH = std::min(NoH, 1.0_r);

	const real alpha2   = m_alpha * m_alpha;
	const real NoH2     = NoH * NoH;
	const real NoH4     = NoH2 * NoH2;
	const real exponent = (NoH2 - 1.0_r) / (NoH2 * alpha2);

	return std::exp(exponent) / (math::constant::pi<real> * alpha2 * NoH4);
}

// TODO: add reference paper
real IsoBeckmann::shadowing(
	const SurfaceHit&     X,
	const math::Vector3R& N,
	const math::Vector3R& H,
	const math::Vector3R& L,
	const math::Vector3R& V) const
{
	const real NoL = N.dot(L);
	const real NoV = N.dot(V);
	const real HoL = H.dot(L);
	const real HoV = H.dot(V);
	if(!isSidednessAgreed(NoL, NoV, HoL, HoV))
	{
		return 0.0_r;
	}
	PH_ASSERT_NE(NoL, 0.0_r);
	PH_ASSERT_NE(NoV, 0.0_r);

	const real NoL2         = std::min(NoL * NoL, 1.0_r);
	const real NoV2         = std::min(NoV * NoV, 1.0_r);
	const real absTanThetaL = math::clamp(
		std::sqrt((1.0_r - NoL2) / NoL2), 0.0_r, std::numeric_limits<real>::max());
	const real absTanThetaV = math::clamp(
		std::sqrt((1.0_r - NoV2) / NoV2), 0.0_r, std::numeric_limits<real>::max());

	const real aL = 1.0_r / (m_alpha * absTanThetaL);
	const real aV = 1.0_r / (m_alpha * absTanThetaV);

	const real lambdaL = aL < 1.6_r ? 
		(1.0_r - 1.259_r * aL + 0.396_r * aL * aL) / (3.535_r * aL + 2.181_r * aL * aL) : 0.0_r;
	const real lambdaV = aV < 1.6_r ? 
		(1.0_r - 1.259_r * aV + 0.396_r * aV * aV) / (3.535_r * aV + 2.181_r * aV * aV) : 0.0_r;

	return 1.0_r / (1.0_r + lambdaL + lambdaV);
}

void IsoBeckmann::genDistributedH(
	const SurfaceHit&          X,
	const math::Vector3R&      N,
	const std::array<real, 2>& sample,
	math::Vector3R* const      out_H) const
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[0], 0.0_r, 1.0_r);
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[1], 0.0_r, 1.0_r);
	PH_ASSERT(out_H);

	// HACK
	if(sample[0] == 1.0_r || sample[1] == 1.0_r)
	{
		out_H->set(N);
		return;
	}

	const real alpha2 = m_alpha * m_alpha;
	const real phi    = math::constant::two_pi<real> * sample[0];
	const real theta  = std::atan(std::sqrt(-alpha2 * std::log(1.0_r - sample[1])));

	// HACK: currently seed can be 1, which should be avoided; it can
	// cause theta to be NaN if alpha = 0 or all kinds of crazy things
	// (may be STL bug since seed should not include 1...)
	if(std::isnan(theta) || std::isinf(theta))
	{
		out_H->set(N);
		return;
	}

	const real sinTheta = std::sin(theta);
	const real cosTheta = std::cos(theta);

	math::Vector3R& H = *out_H;

	H.x = sinTheta * std::sin(phi);
	H.y = cosTheta;
	H.z = sinTheta * std::cos(phi);

	H = X.getDetail().getShadingBasis().localToWorld(H);
	H.normalizeLocal();

	PH_ASSERT_MSG(!std::isnan(H.x) && !std::isnan(H.y) && !std::isnan(H.z) &&
	              !std::isinf(H.x) && !std::isinf(H.y) && !std::isinf(H.z),
		"sample[0] = " + std::to_string(sample[0]) + ", "
		"sample[1] = " + std::to_string(sample[1]) + ", "
		"alpha = "     + std::to_string(m_alpha));
}

}// end namespace ph
