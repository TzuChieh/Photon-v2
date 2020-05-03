#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/Property/DielectricFresnel.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/Spectrum.h"

#include <memory>

namespace ph
{

class IdealDielectric : public SurfaceOptics
{
public:
	explicit IdealDielectric(const std::shared_ptr<DielectricFresnel>& fresnel);
	IdealDielectric(
		const std::shared_ptr<DielectricFresnel>&  fresnel,
		const std::shared_ptr<TTexture<Spectrum>>& reflectionScale,
		const std::shared_ptr<TTexture<Spectrum>>& transmissionScale);

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

private:
	void calcBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const override;

	void calcBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const override;

	void calcBsdfSamplePdfW(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const override;

private:
	constexpr static SurfaceElemental REFLECTION   = 0;
	constexpr static SurfaceElemental TRANSMISSION = 1;

	std::shared_ptr<DielectricFresnel>  m_fresnel;
	std::shared_ptr<TTexture<Spectrum>> m_reflectionScale;
	std::shared_ptr<TTexture<Spectrum>> m_transmissionScale;
};

}// end namespace ph
