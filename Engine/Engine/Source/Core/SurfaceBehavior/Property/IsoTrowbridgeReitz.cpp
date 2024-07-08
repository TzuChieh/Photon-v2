#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"
#include "Math/constant.h"
#include "Math/math.h"
#include "Math/TOrthonormalBasis3.h"

#include <Common/assertion.h>

#include <cmath>
#include <string>

namespace ph
{

IsoTrowbridgeReitz::IsoTrowbridgeReitz(
	const EMaskingShadowing maskingShadowingType)

	: TrowbridgeReitz(maskingShadowingType)
{}

real IsoTrowbridgeReitz::lambda(
	const SurfaceHit& X,
	const math::Vector3R& N,
	const math::Vector3R& H,
	const math::Vector3R& unitDir,
	const std::array<real, 2>& alphas) const
{
	PH_ASSERT_IN_RANGE(unitDir.lengthSquared(), 0.9_r, 1.1_r);
	PH_ASSERT_EQ(alphas[0], alphas[1]);

	const real alpha2 = alphas[0] * alphas[0];
	const real NoD    = N.dot(unitDir);
	const real NoD2   = std::min(NoD * NoD, 1.0_r);
	return 0.5_r * (-1.0_r + std::sqrt(1.0_r + alpha2 * (1.0_r / NoD2 - 1.0_r)));
}

real IsoTrowbridgeReitz::distribution(
	const SurfaceHit& X,
	const math::Vector3R& N, 
	const math::Vector3R& H) const
{
	// Isotropic GGX (Trowbridge-Reitz) normal distribution function
	const auto alphas = getAlphas(X);
	PH_ASSERT_EQ(alphas[0], alphas[1]);

	const real NoH = N.dot(H);
	PH_ASSERT_GE(NoH, -1e-3_r);

	const real alpha  = alphas[0];
	const real alpha2 = alpha * alpha;
	const real NoH2   = NoH * NoH;

	const real innerTerm   = NoH2 * (alpha2 - 1.0_r) + 1.0_r;
	const real denominator = math::constant::pi<real> * innerTerm * innerTerm;

	return alpha2 / denominator;
}

void IsoTrowbridgeReitz::sampleH(
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

	const real alpha = alphas[0];
	const real phi   = math::constant::two_pi<real> * sample[0];

	// Compute theta and handle corner cases like seed == 1 && alpha == 0 (this will cause NaN)
	real theta = std::atan(alpha * std::sqrt(sample[1] / (1.0_r - sample[1])));
	theta = math::safe_clamp(theta, 0.0_r, math::constant::pi<real> * 0.5_r);

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

	PH_ASSERT_MSG(!H.isFinite() || H.lengthSquared() < 0.95_r || H.lengthSquared() > 1.05_r,
		"sample[0] = " + std::to_string(sample[0]) + ", "
		"sample[1] = " + std::to_string(sample[1]) + ", "
		"alpha = "     + std::to_string(alpha));
}

}// end namespace ph
