#pragma once

#include "Core/SurfaceBehavior/Property/TrowbridgeReitz.h"

namespace ph
{

/*! @brief Isotropic Trowbridge-Reitz distribution (GGX).
See the paper "Microfacet Models for Refraction through Rough Surfaces" by Walter et al.
@cite Walter:2007:Microfacet
*/
class IsoTrowbridgeReitz : public TrowbridgeReitz
{
public:
	explicit IsoTrowbridgeReitz(
		EMaskingShadowing maskingShadowingType);

	std::array<real, 2> getAlphas(const SurfaceHit& X) const override = 0;

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
};

}// end namespace ph
