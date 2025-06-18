#pragma once

#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/SurfaceBehavior/Property/DielectricFresnel.h"
#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Math/Color/Spectrum.h"

#include <memory>
#include <vector>

namespace ph
{

class ThinDielectricFilm : public SurfaceOptics
{
public:
	ThinDielectricFilm(
		std::shared_ptr<DielectricFresnel> fresnel,
		std::vector<math::SampledSpectrum> reflectanceTable,
		std::vector<math::SampledSpectrum> transmittanceTable);

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

	void calcBsdfCore(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const override;

	void genBsdfSampleCore(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const override;

	void calcBsdfPdfCore(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const override;

private:
	constexpr static SurfaceElemental REFLECTION   = 0;
	constexpr static SurfaceElemental TRANSMISSION = 1;

	std::shared_ptr<DielectricFresnel> m_fresnel;
	std::vector<math::SampledSpectrum> m_reflectanceTable;
	std::vector<math::SampledSpectrum> m_transmittanceTable;
};

}// end namespace ph
