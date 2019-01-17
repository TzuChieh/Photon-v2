#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"
#include "Common/assertion.h"
#include "Math/math.h"

#include <cmath>
#include <string>

namespace ph
{

IsoTrowbridgeReitz::IsoTrowbridgeReitz(const real alpha) :
	Microfacet()
{
	m_alpha = alpha > 0.001_r ? alpha : 0.001_r;
}

// GGX (Trowbridge-Reitz) Normal Distribution Function
real IsoTrowbridgeReitz::distribution(
	const SurfaceHit& X,
	const Vector3R& N, const Vector3R& H) const
{
	const real NoH = N.dot(H);
	if(NoH <= 0.0_r)
	{
		return 0.0_r;
	}

	const real alpha2 = m_alpha * m_alpha;
	const real NoH2   = NoH * NoH;

	const real innerTerm   = NoH2 * (alpha2 - 1.0_r) + 1.0_r;
	const real denominator = PH_PI_REAL * innerTerm * innerTerm;

	return alpha2 / denominator;
}

// Smith's GGX Geometry Shadowing Function (H is expected to be on the 
// hemisphere of N)
real IsoTrowbridgeReitz::shadowing(
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

	const real alpha2 = m_alpha * m_alpha;
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
	const SurfaceHit& X,
	const real seedA_i0e1, const real seedB_i0e1,
	const Vector3R& N, 
	Vector3R* const out_H) const
{
	PH_ASSERT(seedA_i0e1 >= 0.0_r && seedA_i0e1 <= 1.0_r);
	PH_ASSERT(seedB_i0e1 >= 0.0_r && seedB_i0e1 <= 1.0_r);
	PH_ASSERT(out_H != nullptr);

	const real phi   = 2.0f * PH_PI_REAL * seedA_i0e1;
	const real theta = std::atan(m_alpha * std::sqrt(seedB_i0e1 / (1.0_r - seedB_i0e1)));

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

	Vector3R& H = *out_H;

	H.x = sinTheta * std::sin(phi);
	H.y = cosTheta;
	H.z = sinTheta * std::cos(phi);

	Vector3R xAxis;
	Vector3R yAxis(N);
	Vector3R zAxis;
	math::form_orthonormal_basis(yAxis, &xAxis, &zAxis);
	H = xAxis.mulLocal(H.x).addLocal(yAxis.mulLocal(H.y)).addLocal(zAxis.mulLocal(H.z));
	H.normalizeLocal();

	PH_ASSERT_MSG(!std::isnan(H.x) && !std::isnan(H.y) && !std::isnan(H.z) &&
	              !std::isinf(H.x) && !std::isinf(H.y) && !std::isinf(H.z), "\n"
		"seed-a = " + std::to_string(seedA_i0e1) + "\n"
		"seed-b = " + std::to_string(seedB_i0e1) + "\n"
		"alpha  = " + std::to_string(m_alpha) + "\n");
}

}// end namespace ph