#pragma once

#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/SurfaceBehavior/Property/FresnelEffect.h"
#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Math/Color/Spectrum.h"

#include <memory>

namespace ph
{

class IdealReflector : public SurfaceOptics
{
public:
	explicit IdealReflector(std::shared_ptr<FresnelEffect> fresnel);

	IdealReflector(
		std::shared_ptr<FresnelEffect>            fresnel, 
		std::shared_ptr<TTexture<math::Spectrum>> reflectionScale);

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

	void calcElementalBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const override;

	void genElementalBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const override;

	void calcElementalBsdfPdf(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const override;

private:
	std::shared_ptr<FresnelEffect>            m_fresnel;
	std::shared_ptr<TTexture<math::Spectrum>> m_reflectionScale;
};

}// end namespace ph
