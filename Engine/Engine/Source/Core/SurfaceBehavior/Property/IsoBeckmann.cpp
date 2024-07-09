#include "Core/SurfaceBehavior/Property/IsoBeckmann.h"
#include "Math/constant.h"
#include "Math/math.h"
#include "Math/TOrthonormalBasis3.h"

#include <Common/assertion.h>

#include <cmath>
#include <string>

namespace ph
{

IsoBeckmann::IsoBeckmann(
	const real alpha,
	const EMaskingShadowing maskingShadowingType)

	: ShapeInvariantMicrofacet(maskingShadowingType)
{
	// TODO: test and see if a smaller alpha is possible
	m_alpha = alpha > 0.001_r ? alpha : 0.001_r;
}

real IsoBeckmann::lambda(
	const SurfaceHit& X,
	const math::Vector3R& N,
	const math::Vector3R& H,
	const math::Vector3R& unitDir,
	const std::array<real, 2>& alphas) const
{
	PH_ASSERT_IN_RANGE(unitDir.lengthSquared(), 0.9_r, 1.1_r);
	PH_ASSERT_EQ(alphas[0], alphas[1]);

	const real alpha        = alphas[0];
	const real NoD          = N.dot(unitDir);
	const real NoD2         = std::min(NoD * NoD, 1.0_r);
	const real absTanThetaD = std::sqrt((1.0_r - NoD2) / NoD2);

	const real a = 1.0_r / (alpha * absTanThetaD);

	// An accurate rational approximation from the paper by Walter et al. in 2007
	return a < 1.6_r
		? (1.0_r - 1.259_r * a + 0.396_r * a * a) / (3.535_r * a + 2.181_r * a * a)
		: 0.0_r;
}

real IsoBeckmann::distribution(
	const SurfaceHit& X,
	const math::Vector3R& N,
	const math::Vector3R& H) const
{
	// Isotropic Beckmann normal distribution function
	const auto alphas = getAlphas(X);
	PH_ASSERT_EQ(alphas[0], alphas[1]);

	const real NoH = N.dot(H);
	PH_ASSERT_GE(NoH, -1e-3_r);

	const real alpha2   = alphas[0] * alphas[0];
	const real NoH2     = NoH * NoH;
	const real NoH4     = NoH2 * NoH2;
	const real exponent = (NoH2 - 1.0_r) / (NoH2 * alpha2);

	return std::exp(exponent) / (math::constant::pi<real> * alpha2 * NoH4);
}

void IsoBeckmann::sampleH(
	const SurfaceHit& X,
	const math::Vector3R& N,
	const std::array<real, 2>& sample,
	math::Vector3R* const out_H) const
{
	// Isotropic GGX (Trowbridge-Reitz) normal distribution function
	const auto alphas = getAlphas(X);
	PH_ASSERT_EQ(alphas[0], alphas[1]);

	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[0], 0.0_r, 1.0_r);
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[1], 0.0_r, 1.0_r);

	// Sampling the NDF directly

	const real alpha2 = alphas[0] * alphas[0];
	const real phi    = math::constant::two_pi<real> * sample[0];

	// Compute theta and handle corner cases like seed == 1
	real theta = math::constant::pi<real> * 0.5_r;
	if(1.0_r - sample[1] > 0.0_r)
	{
		theta = std::atan(std::sqrt(-alpha2 * std::log(1.0_r - sample[1])));
	}

	const real sinTheta = std::sin(theta);
	const real cosTheta = std::cos(theta);

	math::Vector3R H(
		sinTheta * std::sin(phi),
		cosTheta,
		sinTheta * std::cos(phi));
	H = X.getDetail().getShadingBasis().localToWorld(H);
	H.normalizeLocal();

	PH_ASSERT(out_H);
	*out_H = H;

	PH_ASSERT_MSG(0.95_r < H.lengthSquared() && H.lengthSquared() < 1.05_r,
		"sample[0] = " + std::to_string(sample[0]) + ", "
		"sample[1] = " + std::to_string(sample[1]) + ", "
		"alpha = "     + std::to_string(m_alpha));
}

}// end namespace ph
