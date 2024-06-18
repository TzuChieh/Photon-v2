#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Math/TVector3.h"
#include "Core/Texture/TTexture.h"
#include "Core/SurfaceBehavior/Property/Microfacet.h"
#include "Core/SurfaceBehavior/Property/ConductorFresnel.h"

#include <memory>

namespace ph
{

/*! @brief Microfacet based opaque surface model.
This model is double-sided.
*/
class OpaqueMicrofacet : public SurfaceOptics
{
public:
	OpaqueMicrofacet(
		const std::shared_ptr<ConductorFresnel>& fresnel,
		const std::shared_ptr<Microfacet>&       microfacet);

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

private:
	std::shared_ptr<Microfacet>    m_microfacet;
	std::shared_ptr<FresnelEffect> m_fresnel;
};

// In-header Implementations:

inline std::string OpaqueMicrofacet::toString() const
{
	return "Opaque Microfacet, " + SurfaceOptics::toString();
}

}// end namespace ph
