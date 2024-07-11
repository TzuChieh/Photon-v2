#include "Core/SurfaceBehavior/Property/AnisoTrowbridgeReitz.h"
#include "Math/math.h"
#include "Math/constant.h"

#include <Common/assertion.h>

#include <cmath>

namespace ph
{

AnisoTrowbridgeReitz::AnisoTrowbridgeReitz(
	const real alphaU,
	const real alphaV,
	const EMaskingShadowing maskingShadowingType)

	: TrowbridgeReitz(maskingShadowingType)
{
	m_alphaU     = alphaU > 0.0001_r ? alphaU : 0.0001_r;
	m_alphaV     = alphaV > 0.0001_r ? alphaV : 0.0001_r;
	m_rcpAlphaU2 = 1.0_r / (m_alphaU * m_alphaU);
	m_rcpAlphaV2 = 1.0_r / (m_alphaV * m_alphaV);
}

real AnisoTrowbridgeReitz::lambda(
	const SurfaceHit& X,
	const math::Vector3R& N,
	const math::Vector3R& H,
	const math::Vector3R& unitDir,
	const std::array<real, 2>& alphas) const
{
	PH_ASSERT_IN_RANGE(unitDir.lengthSquared(), 0.9_r, 1.1_r);

	const real cos2Phi   = X.getDetail().getShadingBasis().cos2Phi(unitDir);
	const real sin2Phi   = 1.0_r - cos2Phi;
	const real alphaU    = alphas[0];
	const real alphaV    = alphas[1];
	const real alpha2    = cos2Phi * alphaU * alphaU + sin2Phi * alphaV * alphaV;
	const real tan2Theta = X.getDetail().getShadingBasis().tan2Theta(unitDir);
	const real sqrtTerm  = 1.0_r + alpha2 * tan2Theta;
	return 0.5_r * (-1.0_r + std::sqrt(sqrtTerm));
}

real AnisoTrowbridgeReitz::distribution(
	const SurfaceHit& X,
	const math::Vector3R& N, 
	const math::Vector3R& H) const
{
	// Anisotropic GGX (Trowbridge-Reitz) normal distribution function
	const auto [alphaU, alphaV] = getAlphas(X);

	const real cosThetaH = N.dot(H);
	PH_ASSERT_GE(cosThetaH, -1e-3_r);

	const real cos2ThetaH = cosThetaH * cosThetaH;
	const real cos4ThetaH = cos2ThetaH * cos2ThetaH;
	const real sin2ThetaH = 1.0_r - cos2ThetaH;
	const real tan2ThetaH = sin2ThetaH / cos2ThetaH;
	const real cos2PhiH   = X.getDetail().getShadingBasis().cos2Phi(H);
	const real sin2PhiH   = 1.0_r - cos2PhiH;
	const real tanTerm    = 1.0_r + tan2ThetaH * (cos2PhiH * m_rcpAlphaU2 + sin2PhiH * m_rcpAlphaV2);
	return 1.0_r / (math::constant::pi<real> * alphaU * alphaV * cos4ThetaH * tanTerm * tanTerm);
}

void AnisoTrowbridgeReitz::sampleH(
	const SurfaceHit& X,
	const math::Vector3R& N,
	const std::array<real, 2>& sample,
	math::Vector3R* const out_H) const
{
	// Anisotropic GGX (Trowbridge-Reitz) normal distribution function
	const auto [alphaU, alphaV] = getAlphas(X);

	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[0], 0.0_r, 1.0_r);
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[1], 0.0_r, 1.0_r);

	const real uFactor = alphaU * std::cos(math::constant::two_pi<real> * sample[0]);
	const real vFactor = alphaV * std::sin(math::constant::two_pi<real> * sample[0]);

	const math::Vector3R zVec(X.getDetail().getShadingBasis().getZAxis().mul(uFactor));
	const math::Vector3R xVec(X.getDetail().getShadingBasis().getXAxis().mul(vFactor));
	
	PH_ASSERT(out_H);
	*out_H = zVec.add(xVec).mul(std::sqrt(sample[1] / (1.0_r - sample[1]))).add(N);
	*out_H = out_H->safeNormalize(N);
}

}// end namespace ph
