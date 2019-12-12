#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Math/TVector3.h"
#include "Core/Texture/TTexture.h"
#include "Core/SurfaceBehavior/Property/Microfacet.h"
#include "Core/SurfaceBehavior/Property/ConductorFresnel.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>

namespace ph
{

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

	void calcBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const override;

	void calcBsdfSamplePdfW(
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
