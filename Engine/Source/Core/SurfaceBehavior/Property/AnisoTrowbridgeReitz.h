#pragma once

#include "Core/SurfaceBehavior/Property/Microfacet.h"

namespace ph
{

class AnisoTrowbridgeReitz : public Microfacet
{
public:
	AnisoTrowbridgeReitz(real alphaU, real alphaV);

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

private:
	real m_alphaU, m_alphaV;
	real m_reciAlphaU2, m_reciAlphaV2;

	real lambda(const SurfaceHit& X, const math::Vector3R& unitDir) const;
};

}// end namespace ph
