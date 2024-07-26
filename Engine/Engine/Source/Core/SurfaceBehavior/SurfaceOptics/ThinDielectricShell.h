#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/Property/DielectricFresnel.h"
#include "Core/Texture/TTexture.h"
#include "Math/Color/Spectrum.h"

#include <Common/primitive_type.h>

#include <memory>

namespace ph
{

class ThinDielectricShell : public SurfaceOptics
{
public:
	explicit ThinDielectricShell(std::shared_ptr<DielectricFresnel> fresnel);

	ThinDielectricShell(
		std::shared_ptr<DielectricFresnel>        fresnel,
		std::shared_ptr<TTexture<real>>           thickness,
		std::shared_ptr<TTexture<math::Spectrum>> sigmaT);

	ThinDielectricShell(
		std::shared_ptr<DielectricFresnel>        fresnel,
		std::shared_ptr<TTexture<math::Spectrum>> reflectionScale,
		std::shared_ptr<TTexture<math::Spectrum>> transmissionScale);

	ThinDielectricShell(
		std::shared_ptr<DielectricFresnel>        fresnel,
		std::shared_ptr<TTexture<real>>           thickness,
		std::shared_ptr<TTexture<math::Spectrum>> sigmaT,
		std::shared_ptr<TTexture<math::Spectrum>> reflectionScale,
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
	constexpr static SurfaceElemental REFLECTION   = 0;
	constexpr static SurfaceElemental TRANSMISSION = 1;

	std::shared_ptr<DielectricFresnel>        m_fresnel;
	std::shared_ptr<TTexture<real>>           m_thickness;
	std::shared_ptr<TTexture<math::Spectrum>> m_sigmaT;
	std::shared_ptr<TTexture<math::Spectrum>> m_reflectionScale;
	std::shared_ptr<TTexture<math::Spectrum>> m_transmissionScale;
};

}// end namespace ph
