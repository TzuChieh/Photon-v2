#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"

#include <cmath>

namespace ph
{

namespace lta
{

/*
	Using shading normal for light transport algorithms is equivalent to using
	asymmetric BSDFs. This can lead to inconsistent results between regular 
	and adjoint algorithms. This correction factor, when multiplied with BSDF,
	restores the symmetric condition of the BSDF during importance transport.

	<Ns>: shading normal
	<Ng>: geometry normal
	<L>:  direction of incident importance
	<V>:  direction of excitant importance

	Note that all vectors are expected to be in unit length and leaving 
	the surface.

	References:

	Eric Veach, "Non-symmetric Scattering in Light Transport Algorithms",
	Eurographics Rendering Workshop 1996 Proceedings.
*/
inline real importance_BSDF_Ns_corrector(
	const Vector3R& Ns, 
	const Vector3R& Ng,
	const Vector3R& L,
	const Vector3R& V)
{
	PH_ASSERT_GT(std::abs(Ns.dot(V) * Ng.dot(L)), 0.0_r);

	return std::abs((Ns.dot(V) * Ng.dot(L)) / (Ng.dot(V) * Ns.dot(L)));
}

}// end namespace lta

}// end namespace ph