#pragma once

#include "Core/SurfaceBehavior/Property/ShapeInvariantMicrofacet.h"

namespace ph
{

/*! @brief Isotropic Beckmann distribution.
See the paper "Microfacet Models for Refraction through Rough Surfaces" by Walter et al.
@cite Walter:2007:Microfacet
*/
class IsoBeckmann : public ShapeInvariantMicrofacet
{
public:
	IsoBeckmann(
		real alpha,
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
	real m_alpha;
};

inline std::array<real, 2> IsoBeckmann::getAlphas(const SurfaceHit& /* X */) const
{
	return {m_alpha, m_alpha};
}

}// end namespace ph
