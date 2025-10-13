#pragma once

#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/Texture/texture_fwd.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/Texture/TSampler.h"

#include <Common/primitive_type.h>

#include <memory>

namespace ph
{

/*! @brief Linearly interpolate between two surface optics.
*/
class LerpedSurfaceOptics : public SurfaceOptics
{
public:
	/*! @brief Computes the final optics as `optics0 * 0.5 + optics1 * 0.5`.
	*/
	LerpedSurfaceOptics(
		const SurfaceOptics* optics0,
		const SurfaceOptics* optics1);

	/*! @brief Computes the final optics as `optics0 * ratio + optics1 * (1 - ratio)`.
	*/
	LerpedSurfaceOptics(
		const SurfaceOptics* optics0,
		const SurfaceOptics* optics1,
		real ratio);

	/*! @brief Computes the final optics as `optics0 * ratio + optics1 * (1 - ratio)`.
	*/
	LerpedSurfaceOptics(
		const SurfaceOptics* optics0,
		const SurfaceOptics* optics1,
		const std::shared_ptr<TTexture<math::Spectrum>>& ratio);

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

	const SurfaceOptics*                      m_optics0;
	const SurfaceOptics*                      m_optics1;
	std::shared_ptr<TTexture<math::Spectrum>> m_ratio;
	TSampler<math::Spectrum>                  m_sampler;
	bool                                      m_containsDelta;
};

// In-header Implementations:

inline std::string LerpedSurfaceOptics::toString() const
{
	return 
		"Lerped Surface Optics, "
		"optics_0: <" + (m_optics0 ? m_optics0->toString() : "null") + ">" +
		"optics_1: <" + (m_optics1 ? m_optics1->toString() : "null") + ">" +
		", " + SurfaceOptics::toString();
}

inline real LerpedSurfaceOptics::probabilityOfPickingOptics0(const math::Spectrum& ratio)
{
	// Depending on the purpose of rendering, favoring human visual system may be preferable,
	// e.g., using luminance. Currently we use the absolute sum just to be fair.
	const real weight0 = ratio.abs().sum();
	const real weight1 = ratio.complement().abs().sum();
	return math::clamp(weight0 / (weight0 + weight1), 0.0_r, 1.0_r);
}

}// end namespace ph
