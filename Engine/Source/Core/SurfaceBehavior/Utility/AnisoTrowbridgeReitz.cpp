#include "Core/SurfaceBehavior/Utility/AnisoTrowbridgeReitz.h"
#include "Math/Math.h"
#include "Core/IntersectionDetail.h"
#include "Math/constant.h"

namespace ph
{

AnisoTrowbridgeReitz::AnisoTrowbridgeReitz(const real alphaX, const real alphaY) :
	m_alphaX(alphaX), m_alphaY(alphaY),
	m_reciAlphaX2(1.0_r / (alphaX * alphaX)), m_reciAlphaY2(1.0_r / (alphaY * alphaY))
{

}

AnisoTrowbridgeReitz::~AnisoTrowbridgeReitz() = default;

real AnisoTrowbridgeReitz::distribution(
	const IntersectionDetail& X,
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
	const real cos2PhiH   = X.getShadingBasis().cos2Phi(H);
	const real sin2PhiH   = 1.0_r - cos2PhiH;

	const real tanTerm = 1.0_r + tan2ThetaH * (cos2PhiH * m_reciAlphaX2 + 
	                                           sin2PhiH * m_reciAlphaY2);
	return 1.0_r / (PH_PI_REAL * m_alphaX * m_alphaY * cos4ThetaH * tanTerm * tanTerm);
}

real AnisoTrowbridgeReitz::shadowing(
	const IntersectionDetail& X,
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
	const IntersectionDetail& X,
	const real seedA_i0e1, const real seedB_i0e1,
	const Vector3R& N,
	Vector3R* const out_H) const
{

}

real AnisoTrowbridgeReitz::lambda(const IntersectionDetail& X, 
                                  const Vector3R& unitDir) const
{
	const real cos2Phi = X.getShadingBasis().cos2Phi(unitDir);
	const real sin2Phi = 1.0_r - cos2Phi;

	const real alpha2    = cos2Phi * m_alphaX * m_alphaX + 
	                       sin2Phi * m_alphaY * m_alphaY;
	const real tan2Theta = X.getShadingBasis().tan2Theta(unitDir);
	const real sqrtTerm  = 1.0_r + alpha2 * tan2Theta;
	return 0.5_r * (-1.0_r + std::sqrt(sqrtTerm));
}

}// end namespace ph