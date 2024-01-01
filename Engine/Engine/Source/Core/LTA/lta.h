#pragma once

#include "Math/TVector3.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cmath>

namespace ph::lta
{

// FIXME: hardcoded number
inline constexpr real self_intersect_delta = 0.0001_r;

/*!
Using shading normal for light transport algorithms is equivalent to using
asymmetric BSDFs. This can lead to inconsistent results between regular
and adjoint algorithms. This correction factor, when multiplied with BSDF,
restores the symmetric condition of the BSDF during importance transport.

References:
[1] Eric Veach, "Non-symmetric Scattering in Light Transport Algorithms",
Eurographics Rendering Workshop 1996 Proceedings.

@param Ns Shading normal.
@param Ng Geometry normal.
@param L Direction of incident importance.
@param V Direction of excitant importance.

@note All vectors are expected to be in unit length and leaving the surface.
*/
inline real importance_BSDF_Ns_corrector(
	const math::Vector3R& Ns,
	const math::Vector3R& Ng,
	const math::Vector3R& L,
	const math::Vector3R& V)
{
	PH_ASSERT_GT(std::abs(Ns.dot(V) * Ng.dot(L)), 0.0_r);

	return std::abs((Ns.dot(V) * Ng.dot(L)) / (Ng.dot(V) * Ns.dot(L)));
}

}// end namespace ph::lta
