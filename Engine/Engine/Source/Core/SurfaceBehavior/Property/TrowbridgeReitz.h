#pragma once

#include "Core/SurfaceBehavior/Property/ShapeInvariantMicrofacet.h"

namespace ph
{

/*! @brief Trowbridge-Reitz distribution (GGX).
See the paper "Microfacet Models for Refraction through Rough Surfaces" by Walter et al.
@cite Walter:2007:Microfacet
*/
class TrowbridgeReitz : public ShapeInvariantMicrofacet
{
public:
	explicit TrowbridgeReitz(
		EMaskingShadowing maskingShadowingType);

	std::array<real, 2> getAlphas(const SurfaceHit& X) const override = 0;

	real lambda(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H,
		const math::Vector3R& unitDir,
		const std::array<real, 2>& alphas) const override = 0;

	real distribution(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H) const override = 0;

	void sampleH(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const std::array<real, 2>& sample,
		math::Vector3R* out_H) const override = 0;

	void sampleVisibleH(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& V,
		const std::array<real, 2>& sample,
		math::Vector3R* out_H) const override;

	lta::PDF pdfSampleVisibleH(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H,
		const math::Vector3R& V) const override;

private:
	/*!
	Sampling visible GGX normals with spherical caps by Dupuy et al. @cite Dupuy:2023:Sampling
	@return A VNDF facet normal (**NOT** normalized), in the same local space as `standardLocalV`.
	*/
	math::Vector3R sampleVNDFDirOnHemisphereDupuy(
		const math::Vector3R& standardLocalV,
		const std::array<real, 2>& sample) const;
};

}// end namespace ph
