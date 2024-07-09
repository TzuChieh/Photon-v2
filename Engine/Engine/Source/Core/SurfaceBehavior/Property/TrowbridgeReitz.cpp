#include "Core/SurfaceBehavior/Property/TrowbridgeReitz.h"
#include "Math/constant.h"
#include "Math/math.h"
#include "Math/TOrthonormalBasis3.h"

#include <Common/assertion.h>

#include <cmath>

namespace ph
{

TrowbridgeReitz::TrowbridgeReitz(
	const EMaskingShadowing maskingShadowingType)

	: ShapeInvariantMicrofacet(maskingShadowingType)
{}

void TrowbridgeReitz::sampleVisibleH(
	const SurfaceHit& X,
	const math::Vector3R& N,
	const math::Vector3R& V,
	const std::array<real, 2>& sample,
	math::Vector3R* const out_H) const
{
	// TODO: for isotropic case, shading basis is not needed,
	// see https://auzaiffe.wordpress.com/2024/04/15/vndf-importance-sampling-an-isotropic-distribution/
	// and https://gist.github.com/jdupuy/4c6e782b62c92b9cb3d13fbb0a5bd7a0#file-samplevndf_ggx-cpp-L51

	const auto upperV = V.dot(N) >= 0.0_r ? V : -V;
	const auto localV = X.getDetail().getShadingBasis().worldToLocal(upperV);

	const auto [alphaU, alphaV] = getAlphas(X);

	// Warp to the hemispherical configuration
	const auto standardLocalV = math::Vector3R(
		localV.x() * alphaV,
		localV.y(),
		localV.z() * alphaU).normalize();

	// Sample the hemisphere
	const auto standardLocalVNDFDir = sampleVNDFDirOnHemisphereDupuy(standardLocalV, sample);

	// Warp back to the ellipsoid configuration (not normalized)
	const math::Vector3R localVNDFDir(
		standardLocalVNDFDir.x() * alphaV,
		standardLocalVNDFDir.y(),
		standardLocalVNDFDir.z() * alphaU);

	const auto worldVNDFDir = X.getDetail().getShadingBasis().localToWorld(localVNDFDir);

	PH_ASSERT(out_H);
	*out_H = worldVNDFDir.safeNormalize(N);
}

lta::PDF TrowbridgeReitz::pdfSampleVisibleH(
	const SurfaceHit& X,
	const math::Vector3R& N,
	const math::Vector3R& H,
	const math::Vector3R& V) const
{
	return {
		.value = visibleDistribution(X, N, H, V),
		.domain = lta::EDomain::HalfSolidAngle};
}

math::Vector3R TrowbridgeReitz::sampleVNDFDirOnHemisphereDupuy(
	const math::Vector3R& standardLocalV,
	const std::array<real, 2>& sample) const
{
	PH_ASSERT_IN_RANGE(standardLocalV.lengthSquared(), 0.95_r, 1.05_r);

	// Sample a spherical cap in (-V.y, 1]
	const real phi      = math::constant::two_pi<real> * sample[0];
	const real y        = std::fma(1.0_r - sample[1], 1.0_r + standardLocalV.y(), -standardLocalV.y());
	const real sinTheta = std::sqrt(math::clamp(1.0_r - y * y, 0.0_r, 1.0_r));
	const real z        = sinTheta * std::cos(phi);
	const real x        = sinTheta * std::sin(phi);

	// Compute halfway direction without normalization (as this is done later)
	return math::Vector3R(x, y, z) + standardLocalV;
}

}// end namespace ph
