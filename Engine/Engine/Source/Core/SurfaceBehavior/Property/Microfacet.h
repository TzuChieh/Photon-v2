#pragma once

#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"

#include <Common/primitive_type.h>

#include <array>

namespace ph
{

class Microfacet
{
public:
	virtual ~Microfacet() = default;

	virtual real distribution(
		const SurfaceHit&     X,
		const math::Vector3R& N,
		const math::Vector3R& H) const = 0;

	virtual real shadowing(
		const SurfaceHit&     X,
		const math::Vector3R& N,
		const math::Vector3R& H,
		const math::Vector3R& L,
		const math::Vector3R& V) const = 0;

	virtual void genDistributedH(
		const SurfaceHit&          X,
		const math::Vector3R&      N,
		const std::array<real, 2>& sample,
		math::Vector3R*            out_H) const = 0;

protected:
	static inline bool isSidednessAgreed(
		const real NoL,
		const real NoV,
		const real HoL,
		const real HoV)
	{
		// The back surface of the microsurface is never visible from directions 
		// on the front side of the macrosurface and vice versa (sidedness agreement)
		return (HoL * NoL > 0.0_r) && (HoV * NoV > 0.0_r);
	}
};

}// end namespace ph
