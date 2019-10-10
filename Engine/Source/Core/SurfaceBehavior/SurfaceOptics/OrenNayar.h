#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Common/primitive_type.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>

namespace ph
{

class OrenNayar : public SurfaceOptics
{
public:
	OrenNayar(
		const std::shared_ptr<TTexture<SpectralStrength>>& albedo,
		real                                               sigmaDegrees);

	OrenNayar(
		const std::shared_ptr<TTexture<SpectralStrength>>& albedo,
		const std::shared_ptr<TTexture<real>>&             sigmaDegrees);

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
	std::shared_ptr<TTexture<real>>             m_sigmaDegrees;
};

// In-header Implementations:

inline std::string OrenNayar::toString() const
{
	return "Oren Nayar, " + SurfaceOptics::toString();
}

}// end namespace ph
