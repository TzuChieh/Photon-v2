#pragma once

#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/SurfaceBehavior/Property/DielectricFresnel.h"
#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Math/Color/Spectrum.h"

#include <memory>

namespace ph
{

class IdealDielectric : public SurfaceOptics
{
public:
	explicit IdealDielectric(std::shared_ptr<DielectricFresnel> fresnel);

	IdealDielectric(
		std::shared_ptr<DielectricFresnel>        fresnel,
		std::shared_ptr<TTexture<math::Spectrum>> reflectionScale,
		std::shared_ptr<TTexture<math::Spectrum>> transmissionScale);

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
	constexpr static SurfaceElemental REFLECTION   = 0;
	constexpr static SurfaceElemental TRANSMISSION = 1;

	std::shared_ptr<DielectricFresnel>        m_fresnel;
	std::shared_ptr<TTexture<math::Spectrum>> m_reflectionScale;
	std::shared_ptr<TTexture<math::Spectrum>> m_transmissionScale;
};

}// end namespace ph
