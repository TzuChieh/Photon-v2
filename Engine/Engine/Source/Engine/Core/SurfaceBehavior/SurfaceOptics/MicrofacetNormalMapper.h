#pragma once

#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/Texture/texture_fwd.h"
#include "Engine/Math/math_fwd.h"
#include "Engine/Core/Texture/TSampler.h"

#include <memory>

namespace ph
{

/*! @brief Microfacet-based normal mapping.
Implements the "normalmap_microfacet_default" model in the original paper.
*/
class MicrofacetNormalMapper : public SurfaceOptics
{
public:
	MicrofacetNormalMapper(
		const SurfaceOptics* target,
		const std::shared_ptr<TTexture<math::Vector3R>>& normalMap);

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

	void calcBsdfCore(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const override;

	void genBsdfSampleCore(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const override;

	void calcBsdfPdfCore(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const override;

	std::string toString() const override;

private:
	math::Vector3R samplePerturbedNormal(const SurfaceHit& X) const;

	const SurfaceOptics*                      m_target;
	std::shared_ptr<TTexture<math::Vector3R>> m_normalMap;
	TSampler<math::Vector3R>                  m_sampler;
};

// In-header Implementations:

inline std::string MicrofacetNormalMapper::toString() const
{
	return 
		"Microfacet Normal Mapper (Surface Optics), "
		"target: <" + (m_target ? m_target->toString() : "null" ) + ">" +
		", " + SurfaceOptics::toString();
}

}// end namespace ph
