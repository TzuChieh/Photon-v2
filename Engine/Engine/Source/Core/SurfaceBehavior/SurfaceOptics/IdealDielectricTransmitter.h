#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/Property/DielectricFresnel.h"
#include "Core/Texture/TTexture.h"
#include "Math/Color/Spectrum.h"

#include <memory>

namespace ph
{

class IdealDielectricTransmitter : public SurfaceOptics
{
public:
	explicit IdealDielectricTransmitter(std::shared_ptr<DielectricFresnel> fresnel);

	IdealDielectricTransmitter(
		std::shared_ptr<DielectricFresnel>        fresnel,
		std::shared_ptr<TTexture<math::Spectrum>> transmissionScale);

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
	std::shared_ptr<DielectricFresnel>        m_fresnel;
	std::shared_ptr<TTexture<math::Spectrum>> m_transmissionScale;
};

}// end namespace ph
