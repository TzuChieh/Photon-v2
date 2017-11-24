#pragma once

#include "Core/SurfaceBehavior/Utility/Microfacet.h"

namespace ph
{

// Reference:
// Microfacet Models for Refraction through Rough Surfaces
// Walter et al., EGSR 2007

class IsoTrowbridgeReitz final : public Microfacet
{
public:
	IsoTrowbridgeReitz(real alpha);
	virtual ~IsoTrowbridgeReitz() override;

	virtual real distribution(const IntersectionDetail& X, 
	                          const Vector3R& N, const Vector3R& H) const override;
	virtual real shadowing(const IntersectionDetail& X, 
	                       const Vector3R& N, const Vector3R& H,
	                       const Vector3R& L, const Vector3R& V) const override;
	virtual void genDistributedH(const IntersectionDetail& X, 
	                             real seedA_i0e1, real seedB_i0e1,
	                             const Vector3R& N, 
	                             Vector3R* out_H) const override;

private:
	real m_alpha;
};

}// end namespace ph