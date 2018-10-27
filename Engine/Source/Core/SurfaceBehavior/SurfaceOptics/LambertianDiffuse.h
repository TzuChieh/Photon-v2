#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>

namespace ph
{

class LambertianDiffuse : public SurfaceOptics
{
public:
	LambertianDiffuse(const std::shared_ptr<TTexture<SpectralStrength>>& albedo);

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
	std::shared_ptr<TTexture<SpectralStrength>> m_albedo;
};

}// end namespace ph