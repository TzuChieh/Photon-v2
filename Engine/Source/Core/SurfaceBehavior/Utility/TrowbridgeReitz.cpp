#include "Core/SurfaceBehavior/Utility/TrowbridgeReitz.h"

#include <cmath>

namespace ph
{

TrowbridgeReitz::TrowbridgeReitz(const real alpha) : 
	Microfacet(),
	m_alpha(alpha)
{

}

TrowbridgeReitz::~TrowbridgeReitz() = default;

// GGX (Trowbridge-Reitz) Normal Distribution Function
real TrowbridgeReitz::distribution(const Vector3R& N, const Vector3R& H) const
{
	const real NoH = N.dot(H);

	if(NoH <= 0.0_r)
	{
		return 0.0_r;
	}

	const real alpha2 = m_alpha * m_alpha;
	const real NoH2   = NoH * NoH;

	const real innerTerm   = NoH2 * (alpha2 - 1.0_r) + 1.0_r;
	const real denominator = PI_REAL * innerTerm * innerTerm;

	return alpha2 / denominator;
}

// Smith's GGX Geometry Shadowing Function (H is expected to be on the hemisphere of N)
real TrowbridgeReitz::shadowing(const Vector3R& N, const Vector3R& H, 
                                const Vector3R& L, const Vector3R& V) const
{
	const real NoL = N.dot(L);
	const real NoV = N.dot(V);
	const real HoL = H.dot(L);
	const real HoV = H.dot(V);

	// The back surface of the microsurface is never visible from directions on the front side 
	// of the macrosurface and viceversa (sidedness agreement)
	if(HoL * NoL <= 0.0_r || HoV * NoV <= 0.0_r)
	{
		return 0.0_r;
	}

	const real alpha2 = m_alpha * m_alpha;

	const real lightG = 2.0_r / (1.0_r + sqrt(alpha2 * (1.0_r / (NoL*NoL) - 1.0_r) + 1.0_r));
	const real viewG  = 2.0_r / (1.0_r + sqrt(alpha2 * (1.0_r / (NoV*NoV) - 1.0_r) + 1.0_r));

	return lightG * viewG;
}

// for GGX (Trowbridge-Reitz) Normal Distribution Function
void TrowbridgeReitz::genDistributedH(const real seedA_i0e1, const real seedB_i0e1,
                                      const Vector3R& N, Vector3R* out_H) const
{
	const real phi   = 2.0f * PI_REAL * seedA_i0e1;
	const real theta = std::atan(m_alpha * std::sqrt(seedB_i0e1 / (1.0_r - seedB_i0e1)));

	const real sinTheta = std::sin(theta);
	const real cosTheta = std::cos(theta);

	Vector3R& H = *out_H;

	H.x = std::cos(phi) * sinTheta;
	H.y = cosTheta;
	H.z = std::sin(phi) * sinTheta;

	Vector3R u;
	Vector3R v(N);
	Vector3R w;
	Math::formOrthonormalBasis(v, &u, &w);
	H = u.mulLocal(H.x).addLocal(v.mulLocal(H.y)).addLocal(w.mulLocal(H.z));
	H.normalizeLocal();
}

}// end namespace ph