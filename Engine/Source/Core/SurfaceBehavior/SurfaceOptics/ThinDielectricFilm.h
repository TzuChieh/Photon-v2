#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/Property/DielectricFresnel.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>
#include <vector>

namespace ph
{

class ThinDielectricFilm : public SurfaceOptics
{
public:
	ThinDielectricFilm(
		const std::shared_ptr<DielectricFresnel>& fresnel,
		const std::vector<SampledSpectralStrength>& reflectanceTable,
		const std::vector<SampledSpectralStrength>& transmittanceTable);

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

	std::shared_ptr<DielectricFresnel>   m_fresnel;
	std::vector<SampledSpectralStrength> m_reflectanceTable;
	std::vector<SampledSpectralStrength> m_transmittanceTable;
};

}// end namespace ph
