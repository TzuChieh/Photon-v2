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
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const override;

	void calcBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		BsdfSample              sample,
		BsdfSampleOutput&       out) const override;

	void calcBsdfSamplePdfW(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const override;

private:
	std::shared_ptr<TTexture<SpectralStrength>> m_albedo;
};

// In-header Implementations:

inline std::string LambertianDiffuse::toString() const
{
	return "Lambertian Diffuse, " + SurfaceOptics::toString();
}

}// end namespace ph
