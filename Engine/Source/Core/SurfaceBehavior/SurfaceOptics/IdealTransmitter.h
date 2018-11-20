#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/Property/DielectricFresnel.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>

namespace ph
{

class IdealTransmitter : public SurfaceOptics
{
public:
	IdealTransmitter(const std::shared_ptr<DielectricFresnel>& fresnel);
	IdealTransmitter(
		const std::shared_ptr<DielectricFresnel>&          fresnel,
		const std::shared_ptr<TTexture<SpectralStrength>>& transmissionScale);

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

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
	std::shared_ptr<DielectricFresnel>          m_fresnel;
	std::shared_ptr<TTexture<SpectralStrength>> m_transmissionScale;
};

}// end namespace ph