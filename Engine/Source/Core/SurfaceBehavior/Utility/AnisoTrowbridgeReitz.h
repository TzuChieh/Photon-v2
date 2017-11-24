#pragma once

#include "Core/SurfaceBehavior/Utility/Microfacet.h"

namespace ph
{

class AnisoTrowbridgeReitz final : public Microfacet
{
public:
	AnisoTrowbridgeReitz(real alphaX, real alphaY);
	virtual ~AnisoTrowbridgeReitz() override;

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
	real m_alphaX, m_alphaY;
	real m_reciAlphaX2, m_reciAlphaY2;

	real lambda(const IntersectionDetail& X, const Vector3R& unitDir) const;
};

}// end namespace ph