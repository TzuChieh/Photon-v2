#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Math/TVector3.h"
#include "Core/Texture/TTexture.h"
#include "Core/SurfaceBehavior/Property/Microfacet.h"
#include "Core/SurfaceBehavior/Property/FresnelEffect.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>

namespace ph
{

class OpaqueMicrofacet : public SurfaceOptics
{
public:
	OpaqueMicrofacet(
		const std::shared_ptr<FresnelEffect>& fresnel,
		const std::shared_ptr<Microfacet>&    microfacet);

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

	/*inline void setAlbedo(const std::shared_ptr<TTexture<SpectralStrength>>& albedo)
	{
		m_albedo = albedo;
	}*/

	std::string toString() const override;

private:
	void calcBsdf(
		const BsdfEvaluation::Input& in,
		BsdfEvaluation::Output&      out,
		const SidednessAgreement&    sidedness) const override;

	void calcBsdfSample(
		const BsdfSample::Input&     in,
		BsdfSample::Output&          out,
		const SidednessAgreement&    sidedness) const override;

	void calcBsdfSamplePdfW(
		const BsdfPdfQuery::Input&   in,
		BsdfPdfQuery::Output&        out,
		const SidednessAgreement&    sidedness) const override;

private:
	//std::shared_ptr<TTexture<SpectralStrength>> m_albedo;
	std::shared_ptr<Microfacet>                 m_microfacet;
	std::shared_ptr<FresnelEffect>              m_fresnel;
};

// In-header Implementations:

inline std::string OpaqueMicrofacet::toString() const
{
	return "Opaque Microfacet, " + SurfaceOptics::toString();
}

}// end namespace ph