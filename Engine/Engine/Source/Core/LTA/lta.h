#pragma once

#include "Math/TVector3.h"
#include "Math/math.h"

#include <Common/config.h>
#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cmath>

namespace ph::lta
{

/*! @brief Transform area domain PDF to solid angle domain PDF w.r.t. a position.
*/
inline real pdfA_to_pdfW(
	const real pdfA,
	const math::Vector3R& dAPosToTargetPos,
	const math::Vector3R& dANormal)
{
	const real distSquared = dAPosToTargetPos.lengthSquared();
	const real signedPdfW = pdfA / dAPosToTargetPos.normalize().dot(dANormal) * distSquared;
	return std::isfinite(signedPdfW) ? std::abs(signedPdfW) : 0.0_r;
}

/*!
Using shading normal for light transport algorithms is equivalent to using
asymmetric BSDFs. This can lead to inconsistent results between regular
and adjoint algorithms. This correction factor, when multiplied with the
weight of an importance scatter event, restores the consistency when adjoint
algorithms also use shading normals during importance transport.

This factor also considers the geometry factor `Ns.dot(L)`. The geometry factor is
expected to be used (multiplied) together with this factor.

References:
[1] Eric Veach, "Non-symmetric Scattering in Light Transport Algorithms",
Eurographics Rendering Workshop 1996 Proceedings.

@param Ns Shading normal.
@param Ng Geometry normal.
@param L Direction of incident importance.
@param V Direction of excitant importance.

@note All vectors are expected to be in unit length and leaving the surface.
*/
inline real importance_scatter_Ns_corrector(
	const math::Vector3R& Ns,
	const math::Vector3R& Ng,
	const math::Vector3R& L,
	const math::Vector3R& V)
{
	PH_ASSERT_NE(Ng.dot(V) * Ns.dot(L), 0.0_r);

	return std::abs((Ns.dot(V) * Ng.dot(L)) / (Ng.dot(V) * Ns.dot(L)));
}

/*!
This correction factor is for the same reason as `importance_scatter_Ns_corrector()`, to restore
the consistency of importance transport when shading normal is used for light transport. This factor
works on the BSDF itself only. Using this factor accounts the fact that shading normals make a BSDF
asymmetric during light/importance transport.

This factor does not take the geometry factor `Ns.dot(L)` into account. It considers the BSDF only.

@param Ns Shading normal.
@param Ng Geometry normal.
@param V Direction of excitant importance.

@note All vectors are expected to be in unit length and leaving the surface.
*/
inline real importance_BSDF_Ns_corrector(
	const math::Vector3R& Ns,
	const math::Vector3R& Ng,
	const math::Vector3R& V)
{
	PH_ASSERT_NE(Ng.dot(V), 0.0_r);

	return std::abs(Ns.dot(V) / Ng.dot(V));
}

/*! @brief Smoother version of `importance_scatter_Ns_corrector()`.

The weighting for importance transport is only correct (same scene interpretation and rendered result)
if we account for the BSDF asymmetry when shading normal is used. However, this factor introduces
high variance (big firefly marks). This version attempts to lower the variance by removing large
correction factor (and this introduces bias, though not easily noticeable).

In PBRT-v3, this factor is completely ignored. In Mitsuba 0.6, this factor is only applied on last
bounce, i.e., during radiance estimation (applying the `importance_BSDF_Ns_corrector()`, not the
scattering/throughput one).
*/
inline real tamed_importance_scatter_Ns_corrector(
	const math::Vector3R& Ns,
	const math::Vector3R& Ng,
	const math::Vector3R& L,
	const math::Vector3R& V)
{
	const auto factor = importance_scatter_Ns_corrector(Ns, Ng, L, V);
#if PH_STRICT_ASYMMETRIC_IMPORTANCE_TRANSPORT
	return factor;
#else
	return math::safe_clamp(factor, 0.0_r, 10.0_r);
#endif
}

/*! @brief Smoother version of `importance_BSDF_Ns_corrector()`.
See `tamed_importance_scatter_Ns_corrector()` for explanation.
*/
inline real tamed_importance_BSDF_Ns_corrector(
	const math::Vector3R& Ns,
	const math::Vector3R& Ng,
	const math::Vector3R& V)
{
	const auto factor = importance_BSDF_Ns_corrector(Ns, Ng, V);
#if PH_STRICT_ASYMMETRIC_IMPORTANCE_TRANSPORT
	return factor;
#else
	// To my testing, the corrector for BSDF dominates the unbearable variance. Clamping to 10 seems to
	// work well. Mitsuba 0.6 uses another approach: discards the photon if `Ng.dot(V) < 1e-2f`.
	return math::safe_clamp(factor, 0.0_r, 10.0_r);
#endif
}

}// end namespace ph::lta
