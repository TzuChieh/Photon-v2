#pragma once

#include "Core/SurfaceBehavior/Property/Microfacet.h"
#include "Core/SurfaceBehavior/Property/enums.h"

#include <Common/assertion.h>

namespace ph
{

/*!
A microfacet distribution that always has the same shape. The `alpha` (mapped from roughness) parameter
will only stretch and scale the shape.
*/
class ShapeInvariantMicrofacet : public Microfacet
{
public:
	explicit ShapeInvariantMicrofacet(
		EMaskingShadowing maskingShadowingType);

	/*!
	@return @f$ \left( \alpha_u, \alpha_v \right) @f$ in the U and V directions of the macrosirface
	parametrization. Guaranteed to have @f$ \alpha_u = \alpha_v @f$ for an isotropic distribution.
	*/
	virtual std::array<real, 2> getAlphas(const SurfaceHit& X) const = 0;

	/*! @brief The @f$ \Lambda \left( a \right) @f$ function that appears in the masking-shadowing term.
	For isotropic distributions, the variable @f$ a @f$ is normally calculated as
	@f$ a = \frac{1}{\alpha \tan \left( \theta \right)} @f$, where
	@f$ \frac{1}{\tan \left( \theta \right)} @f$ is the slope of the unit direction `unitDir` (with
	respect to the macrosurface normal `N`. For anisotropic distributions, see the implementation of
	`AnisoTrowbridgeReitz` as an example for calculating @f$ a @f$ from the parameters.
	@note This method does not handle sidedness agreement.
	*/
	virtual real lambda(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H,
		const math::Vector3R& unitDir,
		const std::array<real, 2>& alphas) const = 0;

	real distribution(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H) const override = 0;

	void sampleH(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const std::array<real, 2>& sample,
		math::Vector3R* out_H) const override = 0;

	real geometry(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H,
		const math::Vector3R& L,
		const math::Vector3R& V) const override;

	lta::PDF pdfSampleH(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H) const override;

protected:
	/*!
	The @f$ G_1 @f$ term of Smith's masking-shadowing function.
	*/
	real smithG1(real lambdaValue) const;

	/*!
	The @f$ \lambda @f$ term for directional correlation. Note that the function is defined in a way
	that 0 corresponds to full correlation and 1 corresponds to no correlation.
	*/
	real empiricalPhiCorrelation(
		const SurfaceHit& X,
		const math::Vector3R& L,
		const math::Vector3R& V) const;

	/*!
	@return Assuming `distribution()` is defined in the half-angle/microfacet space, return the value
	after projecting it onto the macrosurface. This is the normalized form of `distribution()` and
	can be a potential sampling PDF for microfacet normals.
	*/
	real projectedDistribution(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H) const;

	real visibleDistribution(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H,
		const math::Vector3R& V) const;

	EMaskingShadowing m_maskingShadowingType;
};

inline lta::PDF ShapeInvariantMicrofacet::pdfSampleH(
	const SurfaceHit& X,
	const math::Vector3R& N,
	const math::Vector3R& H) const
{
	return {
		.value = projectedDistribution(X, N, H),
		.domain = lta::EDomain::HalfSolidAngle};
}

inline real ShapeInvariantMicrofacet::smithG1(const real lambdaValue) const
{
	return 1.0_r / (1.0_r + lambdaValue);
}

inline real ShapeInvariantMicrofacet::projectedDistribution(
	const SurfaceHit& X,
	const math::Vector3R& N,
	const math::Vector3R& H) const
{
	PH_ASSERT_GE(N.dot(H), -1e-3_r);

	return distribution(X, N, H) * N.absDot(H);
}

inline real ShapeInvariantMicrofacet::visibleDistribution(
	const SurfaceHit& X,
	const math::Vector3R& N,
	const math::Vector3R& H,
	const math::Vector3R& V) const
{
	PH_ASSERT_GE(N.dot(H), -1e-3_r);

	const real lambdaV = lambda(X, N, H, V, getAlphas(X));
	const real g1V     = smithG1(lambdaV);
	const real ndf     = distribution(X, N, H);
	const real vndf    = g1V * ndf * std::abs(H.dot(V) / N.dot(V));

	return vndf;
}

}// end namespace ph
