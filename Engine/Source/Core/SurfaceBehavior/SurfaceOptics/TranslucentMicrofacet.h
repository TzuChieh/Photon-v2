#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/Property/DielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/Microfacet.h"

#include <memory>

namespace ph
{

class TranslucentMicrofacet : public SurfaceOptics
{
public:
	TranslucentMicrofacet(
		const std::shared_ptr<DielectricFresnel>& fresnel, 
		const std::shared_ptr<Microfacet>&        microfacet);

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

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
	std::shared_ptr<DielectricFresnel> m_fresnel;
	std::shared_ptr<Microfacet>        m_microfacet;

	static real getReflectionProbability(const SpectralStrength& F);

	constexpr static SurfaceElemental REFLECTION   = 0;
	constexpr static SurfaceElemental TRANSMISSION = 1;
};

// In-header Implementations:

inline std::string TranslucentMicrofacet::toString() const
{
	return "Translucent Microfacet, " + SurfaceOptics::toString();
}

}// end namespace ph