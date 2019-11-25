#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/Property/FresnelEffect.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>

namespace ph
{

class IdealReflector : public SurfaceOptics
{
public:
	IdealReflector(const std::shared_ptr<FresnelEffect>& fresnel);
	IdealReflector(
		const std::shared_ptr<FresnelEffect>&              fresnel, 
		const std::shared_ptr<TTexture<SpectralStrength>>& reflectionScale);

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

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
	std::shared_ptr<FresnelEffect>              m_fresnel;
	std::shared_ptr<TTexture<SpectralStrength>> m_reflectionScale;
};

}// end namespace ph
