#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/Property/FresnelEffect.h"
#include "Core/Texture/TTexture.h"
#include "Math/Color/Spectrum.h"

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

private:
	void calcBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const override;

	void genBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const override;

	void calcBsdfPdf(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const override;

private:
	std::shared_ptr<FresnelEffect>            m_fresnel;
	std::shared_ptr<TTexture<math::Spectrum>> m_reflectionScale;
};

}// end namespace ph
