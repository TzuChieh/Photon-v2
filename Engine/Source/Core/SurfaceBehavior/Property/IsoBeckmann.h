#pragma once

#include "Core/SurfaceBehavior/Property/Microfacet.h"

namespace ph
{

// Reference:
// Microfacet Models for Refraction through Rough Surfaces
// Walter et al., EGSR 2007
class IsoBeckmann : public Microfacet
{
public:
	explicit IsoBeckmann(real alpha);

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
		const SurfaceHit&     X,
		real                  seedA_i0e1, 
		real                  seedB_i0e1,
		const math::Vector3R& N,
		math::Vector3R*       out_H) const override;

private:
	real m_alpha;
};

}// end namespace ph
