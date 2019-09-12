#pragma once

#include "Core/SurfaceBehavior/Property/Microfacet.h"

namespace ph
{

class AnisoTrowbridgeReitz : public Microfacet
{
public:
	AnisoTrowbridgeReitz(real alphaU, real alphaV);

	real distribution(const SurfaceHit& X,
	                  const Vector3R& N, const Vector3R& H) const override;

	real shadowing(const SurfaceHit& X,
	               const Vector3R& N, const Vector3R& H,
	               const Vector3R& L, const Vector3R& V) const override;

	void genDistributedH(const SurfaceHit& X,
	                     real seedA_i0e1, real seedB_i0e1,
	                     const Vector3R& N,
	                     Vector3R* out_H) const override;

private:
	real m_alphaU, m_alphaV;
	real m_reciAlphaU2, m_reciAlphaV2;

	real lambda(const SurfaceHit& X, const Vector3R& unitDir) const;
};

}// end namespace ph
