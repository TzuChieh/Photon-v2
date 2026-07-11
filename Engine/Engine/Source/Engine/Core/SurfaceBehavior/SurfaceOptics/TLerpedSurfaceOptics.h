#pragma once

#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"
#include "Engine/Math/Color/Spectrum.h"

#include <Common/compiler.h>
#include <Common/primitive_type.h>

#include <concepts>
#include <utility>

namespace ph
{

/*! @brief Linearly interpolate between two surface optics.
*/
template<typename Factor>
class TLerpedSurfaceOptics : public SurfaceOptics
{
	static_assert(CSurfaceProperty<Factor, math::Spectrum>,
		"Factor must accept SurfaceHit and return a spectrum-convertible value.");

public:
	/*! @brief Computes the final optics as `optics0 * 0.5 + optics1 * 0.5`.
	*/
	TLerpedSurfaceOptics(
		const SurfaceOptics* optics0,
		const SurfaceOptics* optics1)

		: TLerpedSurfaceOptics(
			optics0,
			optics1,
			0.5_r)
	{}

	/*! @brief Computes the final optics as `optics0 * ratio + optics1 * (1 - ratio)`.
	*/
	TLerpedSurfaceOptics(
		const SurfaceOptics* optics0,
		const SurfaceOptics* optics1,
		real ratio)

		requires std::constructible_from<Factor, math::Spectrum>
		
		: TLerpedSurfaceOptics(
			optics0,
			optics1,
			Factor(math::Spectrum(ratio)))
	{}

	/*! @brief Computes the final optics as `optics0 * ratio + optics1 * (1 - ratio)`.
	*/
	TLerpedSurfaceOptics(
		const SurfaceOptics* optics0,
		const SurfaceOptics* optics1,
		Factor factor);

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

	void calcElementalBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const override;

	void genElementalBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const override;

	void calcElementalBsdfPdf(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const override;

	std::string toString() const override;

private:
	static real probabilityOfPickingOptics0(const math::Spectrum& ratio);

	const SurfaceOptics* m_optics0;
	const SurfaceOptics* m_optics1;

	[[PH_NO_UNIQUE_ADDRESS]]
	Factor m_factor;

	bool m_containsDelta;
};

// In-header Implementations:

template<typename Factor>
inline std::string TLerpedSurfaceOptics<Factor>::toString() const
{
	return 
		"Lerped Surface Optics, "
		"optics_0: <" + (m_optics0 ? m_optics0->toString() : "null") + ">" +
		"optics_1: <" + (m_optics1 ? m_optics1->toString() : "null") + ">" +
		", " + SurfaceOptics::toString();
}

template<typename Factor>
inline real TLerpedSurfaceOptics<Factor>::probabilityOfPickingOptics0(
	const math::Spectrum& ratio)
{
	// Depending on the purpose of rendering, favoring human visual system may be preferable,
	// e.g., using luminance. Currently we use the absolute sum just to be fair.
	const real weight0 = ratio.abs().sum();
	const real weight1 = ratio.complement().abs().sum();
	return math::clamp(weight0 / (weight0 + weight1), 0.0_r, 1.0_r);
}

}// end namespace ph

#include "Engine/Core/SurfaceBehavior/SurfaceOptics/TLerpedSurfaceOptics.ipp"
