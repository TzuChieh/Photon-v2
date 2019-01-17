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
	explicit LambertianDiffuse(const std::shared_ptr<TTexture<SpectralStrength>>& albedo);

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
	std::shared_ptr<TTexture<SpectralStrength>> m_albedo;
};

// In-header Implementations:

inline std::string LambertianDiffuse::toString() const
{
	return "Lambertian Diffuse, " + SurfaceOptics::toString();
}

}// end namespace ph