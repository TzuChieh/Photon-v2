#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"
#include "Common/assertion.h"
#include "Math/math.h"
#include "Math/TOrthonormalBasis3.h"

#include <cmath>
#include <string>

namespace ph
{

// GGX (Trowbridge-Reitz) Normal Distribution Function
real IsoTrowbridgeReitz::distribution(
	const SurfaceHit&     X,
	const math::Vector3R& N, 
	const math::Vector3R& H) const
{
	const real NoH = N.dot(H);
	if(NoH <= 0.0_r)
	{
		return 0.0_r;
	}

	const real alpha  = getAlpha(X);
	const real alpha2 = alpha * alpha;
	const real NoH2   = NoH * NoH;

	const real innerTerm   = NoH2 * (alpha2 - 1.0_r) + 1.0_r;
	const real denominator = math::constant::pi<real> * innerTerm * innerTerm;

	return alpha2 / denominator;
}

// Smith's GGX Geometry Shadowing Function (H is expected to be on the 
// hemisphere of N)
real IsoTrowbridgeReitz::shadowing(
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

	const real alpha  = getAlpha(X);
	const real alpha2 = alpha * alpha;
	const real NoL2   = NoL * NoL;
	const real NoV2   = NoV * NoV;

	const real lambdaL = 0.5_r * (-1.0_r + std::sqrt(1.0_r + alpha2 * (1.0_r / NoL2 - 1.0_r)));
	const real lambdaV = 0.5_r * (-1.0_r + std::sqrt(1.0_r + alpha2 * (1.0_r / NoV2 - 1.0_r)));

	// Here the shadowing factor uses a more accurate version than the one used
	// by Walter et al. (2007).
	//
	// Reference: Eric Heitz, Understanding the Masking-Shadowing Function in 
	// Microfacet Based BRDFs, Journal of Computer Graphics Techniques Vol. 3, 
	// No. 2, 2014. Equation 99 is the one used here.

	return 1.0_r / (1.0_r + lambdaL + lambdaV);
}

// for GGX (Trowbridge-Reitz) Normal Distribution Function
//
void IsoTrowbridgeReitz::genDistributedH(
	const SurfaceHit&          X,
	const math::Vector3R&      N,
	const std::array<real, 2>& sample,
	math::Vector3R* const      out_H) const
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[0], 0.0_r, 1.0_r);
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[1], 0.0_r, 1.0_r);
	PH_ASSERT(out_H);

	const real alpha = getAlpha(X);
	const real phi   = math::constant::two_pi<real> * sample[0];
	const real theta = std::atan(alpha * std::sqrt(sample[1] / (1.0_r - sample[1])));

	// HACK: currently seed can be 1, which should be avoided; it can
	// cause theta to be NaN if alpha = 0 or all kinds of crazy things
	// (may be STL bug since seed should not include 1...)
	if(std::isnan(theta) || std::isinf(theta))
	{
		*out_H = N;
		return;
	}

	const real sinTheta = std::sin(theta);
	const real cosTheta = std::cos(theta);

	math::Vector3R& H = *out_H;

	H.x() = sinTheta * std::sin(phi);
	H.y() = cosTheta;
	H.z() = sinTheta * std::cos(phi);

	H = X.getDetail().getShadingBasis().localToWorld(H);
	H.normalizeLocal();

	PH_ASSERT_MSG(!std::isnan(H.x()) && !std::isnan(H.y()) && !std::isnan(H.z()) &&
	              !std::isinf(H.x()) && !std::isinf(H.y()) && !std::isinf(H.z()),
		"sample[0] = " + std::to_string(sample[0]) + ", "
		"sample[1] = " + std::to_string(sample[1]) + ", "
		"alpha = "     + std::to_string(alpha));
}

}// end namespace ph
