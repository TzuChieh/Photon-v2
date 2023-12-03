#pragma once

#include "Core/SurfaceBehavior/Property/Microfacet.h"

namespace ph
{

// Reference:
// Microfacet Models for Refraction through Rough Surfaces
// Walter et al., EGSR 2007
class IsoTrowbridgeReitz : public Microfacet
{
public:
	virtual real getAlpha(const SurfaceHit& X) const = 0;

	real distribution(
		const SurfaceHit&     X,
		const math::Vector3R& N,
		const math::Vector3R& H) const override;

	real shadowing(
		const SurfaceHit&     X,
		const math::Vector3R& N,
		const math::Vector3R& H,
		const math::Vector3R& L,
		const math::Vector3R& V) const override;

	void genDistributedH(
		const SurfaceHit&          X,
		const math::Vector3R&      N,
		const std::array<real, 2>& sample,
		math::Vector3R*            out_H) const override;
};

}// end namespace ph
