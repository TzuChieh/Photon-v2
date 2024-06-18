#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Texture/texture_fwd.h"
#include "Math/Color/Spectrum.h"
#include "Core/Texture/TSampler.h"

#include <Common/primitive_type.h>

#include <memory>
#include <vector>

namespace ph
{

// FIXME: lerping between delta distribution(s) seems broken
class LerpedSurfaceOptics : public SurfaceOptics
{
public:
	LerpedSurfaceOptics(
		const std::shared_ptr<SurfaceOptics>& optics0, 
		const std::shared_ptr<SurfaceOptics>& optics1);

	/*! @brief Computes the final optics as `optics0 * ratio + optics1 * (1 - ratio)`.
	*/
	LerpedSurfaceOptics(
		const std::shared_ptr<SurfaceOptics>& optics0,
		const std::shared_ptr<SurfaceOptics>& optics1,
		real ratio);

	/*! @brief Computes the final optics as `optics0 * ratio + optics1 * (1 - ratio)`.
	*/
	LerpedSurfaceOptics(
		const std::shared_ptr<SurfaceOptics>& optics0, 
		const std::shared_ptr<SurfaceOptics>& optics1,
		const std::shared_ptr<TTexture<math::Spectrum>>& ratio);

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

	std::string toString() const override;

private:
	void calcBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const override;

	void genBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const override;

	void calcBsdfPdf(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const override;

	static real probabilityOfPickingOptics0(const math::Spectrum& ratio);

	std::shared_ptr<SurfaceOptics>            m_optics0;
	std::shared_ptr<SurfaceOptics>            m_optics1;
	std::shared_ptr<TTexture<math::Spectrum>> m_ratio;
	TSampler<math::Spectrum>                  m_sampler;
	bool                                      m_containsDelta;
};

// In-header Implementations:

inline std::string LerpedSurfaceOptics::toString() const
{
	return 
		"Lerped Surface Optics, "
		"optics_0: " + m_optics0->toString() + 
		"optics_1: " + m_optics1->toString() + 
		", " + SurfaceOptics::toString();
}

inline real LerpedSurfaceOptics::probabilityOfPickingOptics0(const math::Spectrum& ratio)
{
	return math::clamp(ratio.relativeLuminance(math::EColorUsage::ECF), 0.0_r, 1.0_r);
}

}// end namespace ph
