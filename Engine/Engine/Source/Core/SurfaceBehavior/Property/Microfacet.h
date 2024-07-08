#pragma once

#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/LTA/PDF.h"

#include <Common/primitive_type.h>

#include <array>

namespace ph
{

/*!
Encapsulates a microfacet distribution along with its properties and common routines.

Regarding the microfacet normal, it is denoted as `H` (for half vector) throughout the implementation.
`H` is expected to be on the hemisphere of `N`, though a small error (slightly out-of-hemisphere)
can exist. As `N`, `H` is expected to be normalized.
*/
class Microfacet
{
public:
	virtual ~Microfacet() = default;

	/*! @brief Distribution of the microfacet normal.
	The @f$ D @f$ term. Also commonly knwon as the NDF (normal distribution function). This term is
	defined in the half-angle/microfacet space (the angle between `N` and `H`) unless otherwise noted.
	@param H The microfacet normal.
	@remark Do not treat this term as the probability density of microfacet normal over solid angle.
	*/
	virtual real distribution(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H) const = 0;

	/*! @brief Masking and shadowing due to nearby microfacets.
	The @f$ G @f$ term.
	*/
	virtual real geometry(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H,
		const math::Vector3R& L,
		const math::Vector3R& V) const = 0;

	/*! @brief Generate a microfacet normal `H` for the distribution.
	This samples all possible `H` vectors for the distribution.
	@param out_H The generated microfacet normal.
	@remark Use `pdfSampleH()` for the probability density of sampling the generated `H`.
	`distribution()` is not a probability density of microfacet normal.
	*/
	virtual void sampleH(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const std::array<real, 2>& sample,
		math::Vector3R* out_H) const = 0;

	/*!
	@return PDF of generating the specific microfacet normal `H` using `sampleH()`.
	*/
	virtual lta::PDF pdfSampleH(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H) const = 0;

	/*! @brief Same as `sampleH()`, but tries to take geometry term into consideration.
	This samples only potentially visible `H` vectors for the distribution. If unable to fully or
	partly incorporate visibility information from the geometry term, this method will fallback
	to `sampleH()`.
	*/
	virtual void sampleVisibleH(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& V,
		const std::array<real, 2>& sample,
		math::Vector3R* out_H) const;

	/*!
	@return PDF of generating the specific microfacet normal `H` using `sampleVisibleH()`.
	*/
	virtual lta::PDF pdfSampleVisibleH(
		const SurfaceHit& X,
		const math::Vector3R& N,
		const math::Vector3R& H,
		const math::Vector3R& V) const;

protected:
	/*!
	@param NoD Dot of the macrosurface normal `N` and a unit direction `D`.
	@param HoD Dot of the microsurface normal `H` and a unit direction `D`.
	@return Checks the condition that the back surface of the microsurface is never visible from
	directions (such as `D`) on the front side of the macrosurface and vice versa (sidedness agreement).
	This is a slightly modified version from the paper by Walter et al. @cite Walter:2007:Microfacet,
	and is somewhat stricter when applied on an opaque surface (due to the additional requirement on
	the relation with macrosurface normal).
	*/
	static bool isSidednessAgreed(
		real NoD,
		real HoD);

	/*!
	Same as `isSidednessAgreed(2)`, but checks for both incident direction `L` and outgoing direction `V`.
	*/
	static bool isSidednessAgreed(
		real NoL,
		real NoV,
		real HoL,
		real HoV);
};

inline void Microfacet::sampleVisibleH(
	const SurfaceHit& X,
	const math::Vector3R& N,
	const math::Vector3R& /* V */,
	const std::array<real, 2>& sample,
	math::Vector3R* const out_H) const
{
	return sampleH(X, N, sample, out_H);
}

inline lta::PDF Microfacet::pdfSampleVisibleH(
	const SurfaceHit& X,
	const math::Vector3R& N,
	const math::Vector3R& H,
	const math::Vector3R& /* V */) const
{
	return pdfSampleH(X, N, H);
}

inline bool Microfacet::isSidednessAgreed(
	const real NoD,
	const real HoD)
{
	return HoD * NoD > 0.0_r;
}

inline bool Microfacet::isSidednessAgreed(
	const real NoL,
	const real NoV,
	const real HoL,
	const real HoV)
{
	return isSidednessAgreed(NoL, HoL) && isSidednessAgreed(NoV, HoV);
}

}// end namespace ph
