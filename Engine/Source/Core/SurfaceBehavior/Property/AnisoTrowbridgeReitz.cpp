#include "Core/SurfaceBehavior/Property/AnisoTrowbridgeReitz.h"
#include "Math/math.h"
#include "Math/constant.h"
#include "Common/assertion.h"

#include <cmath>

namespace ph
{

AnisoTrowbridgeReitz::AnisoTrowbridgeReitz(const real alphaU, const real alphaV) :
	Microfacet()
{
	// TODO: clamping is unnecessary if alpha is properly mapped from roughness
	m_alphaU      = alphaU > 0.0001_r ? alphaU : 0.0001_r;
	m_alphaV      = alphaV > 0.0001_r ? alphaV : 0.0001_r;
	m_reciAlphaU2 = 1.0_r / (m_alphaU * m_alphaU);
	m_reciAlphaV2 = 1.0_r / (m_alphaV * m_alphaV);
}

real AnisoTrowbridgeReitz::distribution(
	const SurfaceHit& X,
	const Vector3R& N, const Vector3R& H) const
{
	real cosThetaH = N.dot(H);
	if(cosThetaH <= 0.0_r)
	{
		return 0.0_r;
	}

	cosThetaH = cosThetaH > 1.0_r ? 1.0_r : cosThetaH;

	const real cos2ThetaH = cosThetaH * cosThetaH;
	const real cos4ThetaH = cos2ThetaH * cos2ThetaH;
	const real sin2ThetaH = 1.0_r - cos2ThetaH;
	const real tan2ThetaH = sin2ThetaH / cos2ThetaH;
	const real cos2PhiH   = X.getDetail().getShadingBasis().cos2Phi(H);
	const real sin2PhiH   = 1.0_r - cos2PhiH;

	const real tanTerm = 1.0_r + tan2ThetaH * (cos2PhiH * m_reciAlphaU2 + 
	                                           sin2PhiH * m_reciAlphaV2);
	const real D = 1.0_r / (constant::pi<real> * m_alphaU * m_alphaV * cos4ThetaH * tanTerm * tanTerm);
	return std::isfinite(D) ? D : 0;
}

real AnisoTrowbridgeReitz::shadowing(
	const SurfaceHit& X,
	const Vector3R& N, const Vector3R& H,
	const Vector3R& L, const Vector3R& V) const
{
	const real NoL = N.dot(L);
	const real NoV = N.dot(V);
	const real HoL = H.dot(L);
	const real HoV = H.dot(V);
	if(!isSidednessAgreed(NoL, NoV, HoL, HoV))
	{
		return 0.0_r;
	}

	return 1.0_r / (1.0_r + lambda(X, L) + lambda(X, V));
}

void AnisoTrowbridgeReitz::genDistributedH(
	const SurfaceHit& X,
	const real seedA_i0e1, const real seedB_i0e1,
	const Vector3R& N,
	Vector3R* const out_H) const
{
	PH_ASSERT(seedA_i0e1 >= 0.0_r && seedA_i0e1 <= 1.0_r);
	PH_ASSERT(seedB_i0e1 >= 0.0_r && seedB_i0e1 <= 1.0_r);
	PH_ASSERT(out_H != nullptr);

	const real uFactor = m_alphaU * std::cos(constant::two_pi<real> * seedA_i0e1);
	const real vFactor = m_alphaV * std::sin(constant::two_pi<real> * seedA_i0e1);

	const Vector3R zVec(X.getDetail().getShadingBasis().zAxis.mul(uFactor));
	const Vector3R xVec(X.getDetail().getShadingBasis().xAxis.mul(vFactor));
	
	out_H->set(zVec.add(xVec).mul(std::sqrt(seedB_i0e1 / (1.0_r - seedB_i0e1))).add(N));
	out_H->normalizeLocal();
}

real AnisoTrowbridgeReitz::lambda(const SurfaceHit& X,
                                  const Vector3R& unitDir) const
{
	const real cos2Phi = X.getDetail().getShadingBasis().cos2Phi(unitDir);
	const real sin2Phi = 1.0_r - cos2Phi;

	const real alpha2    = cos2Phi * m_alphaU * m_alphaU + 
	                       sin2Phi * m_alphaV * m_alphaV;
	const real tan2Theta = X.getDetail().getShadingBasis().tan2Theta(unitDir);
	const real sqrtTerm  = 1.0_r + alpha2 * tan2Theta;
	return 0.5_r * (-1.0_r + std::sqrt(sqrtTerm));
}

}// end namespace ph