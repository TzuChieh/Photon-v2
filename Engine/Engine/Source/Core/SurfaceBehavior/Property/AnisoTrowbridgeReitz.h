#pragma once

#include "Core/SurfaceBehavior/Property/TrowbridgeReitz.h"

namespace ph
{

/*!
See the original paper by Trowbridge et al. @cite Trowbridge:1975:Average. The course note
@cite Burley:2012:Physicallybased provides an in-depth description for practical applications.
*/
class AnisoTrowbridgeReitz : public TrowbridgeReitz
{
public:
	AnisoTrowbridgeReitz(
		real alphaU,
		real alphaV,
		EMaskingShadowing maskingShadowingType);

	std::array<real, 2> getAlphas(const SurfaceHit& X) const override;

	real lambda(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H,
		const math::Vector3R& unitDir,
		const std::array<real, 2>& alphas) const override;

	real distribution(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H) const override;

	void sampleH(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const std::array<real, 2>& sample,
		math::Vector3R* out_H) const override;

private:
	real m_alphaU;
	real m_alphaV;
	real m_rcpAlphaU2;
	real m_rcpAlphaV2;
};

inline std::array<real, 2> AnisoTrowbridgeReitz::getAlphas(const SurfaceHit& /* X */) const
{
	return {m_alphaU, m_alphaV};
}

}// end namespace ph
