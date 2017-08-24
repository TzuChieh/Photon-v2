#pragma once

#include "Core/SurfaceBehavior/Utility/Microfacet.h"

namespace ph
{

class TrowbridgeReitz : public Microfacet
{
public:
	TrowbridgeReitz(real alpha);
	virtual ~TrowbridgeReitz() override;

	virtual real distribution(const Vector3R& N, const Vector3R& H) const override;
	virtual real shadowing(const Vector3R& N, const Vector3R& H, 
	                       const Vector3R& L, const Vector3R& V) const override;
	virtual void genDistributedH(real seedA_i0e1, real seedB_i0e1, 
	                             const Vector3R& N, Vector3R* out_H) const override;

private:
	real m_alpha;
};

}// end namespace ph