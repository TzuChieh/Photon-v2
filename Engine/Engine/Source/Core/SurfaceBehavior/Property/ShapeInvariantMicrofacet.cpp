#include "Core/SurfaceBehavior/Property/ShapeInvariantMicrofacet.h"
#include "Math/constant.h"
#include "Math/math.h"

#include <Common/assertion.h>

#include <cmath>

namespace ph
{

ShapeInvariantMicrofacet::ShapeInvariantMicrofacet(
	const EMaskingShadowing maskingShadowingType)

	: Microfacet()

	, m_maskingShadowingType(maskingShadowingType)
{}

real ShapeInvariantMicrofacet::geometry(
	const SurfaceHit& X,
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

	const auto alphas  = getAlphas(X);
	const real lambdaL = lambda(X, N, H, L, alphas);
	const real lambdaV = lambda(X, N, H, V, alphas);

	real g = 0.0_r;
	switch(m_maskingShadowingType)
	{
	case EMaskingShadowing::Separable:
	{
		// Smith's G for the geometry masking-shadowing function
		g = smithG1(lambdaL) * smithG1(lambdaV);
	}
		break;

	case EMaskingShadowing::HightCorrelated:
	{
		// This is a more accurate version than the one used by Walter et al. (2007).
		//
		// Reference: Eric Heitz, Understanding the Masking-Shadowing Function in 
		// Microfacet Based BRDFs, Journal of Computer Graphics Techniques Vol. 3, 
		// No. 2, 2014. Equation 99 is the one used here.
		g = 1.0_r / (1.0_r + lambdaL + lambdaV);
	}
		break;

	case EMaskingShadowing::DirectionCorrelated:
	{
		const real phiCorrelation = empiricalPhiCorrelation(X, L, V);
		const real g1L            = smithG1(lambdaL);
		const real g1V            = smithG1(lambdaV);
		g = phiCorrelation * g1L * g1V + (1.0_r - phiCorrelation) * std::min(g1L, g1V);
	}
		break;

	case EMaskingShadowing::HeightDirectionCorrelated:
	{
		const real phiCorrelation = empiricalPhiCorrelation(X, L, V);
		g = 1.0_r / (1.0_r + std::max(lambdaL, lambdaV) + phiCorrelation * std::min(lambdaL, lambdaV));
	}
		break;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		g = 0.0_r;
		break;
	}

	return g;
}

real ShapeInvariantMicrofacet::empiricalPhiCorrelation(
	const SurfaceHit& X,
	const math::Vector3R& L,
	const math::Vector3R& V) const
{
	const auto projectedL  = X.getDetail().getShadingBasis().sinTheta(L) * L;
	const auto projectedV  = X.getDetail().getShadingBasis().sinTheta(V) * V;
	const real cosDeltaPhi = math::clamp(projectedL.dot(projectedV), -1.0_r, 1.0_r);

	const real deltaPhi = std::acos(cosDeltaPhi);
	PH_ASSERT_IN_RANGE_INCLUSIVE(deltaPhi, 0.0_r, math::constant::pi<real>);

	const real correlation = (4.41_r * deltaPhi) / (4.41_r * deltaPhi + 1);
	PH_ASSERT_IN_RANGE_INCLUSIVE(correlation, 0.0_r, 1.0_r);

	return correlation;
}

}// end namespace ph
