#include "Core/SurfaceBehavior/SurfaceOptics/LerpedSurfaceOptics.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Math/Random.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Common/assertion.h"
#include "Core/Texture/TConstantTexture.h"

#include "Math/math.h"

namespace ph
{

LerpedSurfaceOptics::LerpedSurfaceOptics(
	const std::shared_ptr<SurfaceOptics>& optics0,
	const std::shared_ptr<SurfaceOptics>& optics1) : 

	LerpedSurfaceOptics(
		optics0,
		optics1,
		0.5_r)
{}

LerpedSurfaceOptics::LerpedSurfaceOptics(
	const std::shared_ptr<SurfaceOptics>& optics0,
	const std::shared_ptr<SurfaceOptics>& optics1,
	const real ratio) : 

	LerpedSurfaceOptics(
		optics0, 
		optics1, 
		std::make_shared<TConstantTexture<SpectralStrength>>(SpectralStrength(ratio)))
{}

LerpedSurfaceOptics::LerpedSurfaceOptics(
	const std::shared_ptr<SurfaceOptics>& optics0,
	const std::shared_ptr<SurfaceOptics>& optics1,
	const std::shared_ptr<TTexture<SpectralStrength>>& ratio) : 

	m_optics0(optics0),
	m_optics1(optics1),
	m_ratio(ratio),
	m_sampler(EQuantity::ECF)
{
	PH_ASSERT(optics0 != nullptr && optics1 != nullptr && ratio != nullptr);

	m_phenomena.set({});
	m_phenomena.unionWith(m_optics0->m_phenomena);
	m_phenomena.unionWith(m_optics1->m_phenomena);
}

void LerpedSurfaceOptics::calcBsdf(
	const BsdfEvaluation::Input& in,
	BsdfEvaluation::Output&      out,
	const SidednessAgreement&    sidedness) const
{
	const SpectralStrength ratio = m_sampler.sample(*m_ratio, in.X);

	BsdfEvaluation::Output eval0, eval1;
	m_optics0->calcBsdf(in, eval0, sidedness);
	m_optics1->calcBsdf(in, eval1, sidedness);

	out.bsdf = eval0.bsdf * ratio + eval1.bsdf * (SpectralStrength(1) - ratio);
}

void LerpedSurfaceOptics::calcBsdfSample(
	const BsdfSample::Input&  in,
	BsdfSample::Output&       out,
	const SidednessAgreement& sidedness) const
{
	const SpectralStrength ratio = m_sampler.sample(*m_ratio, in.X);
	const real             prob  = probabilityOfPickingOptics0(ratio);

	const real dart = Random::genUniformReal_i0_e1();
	if(dart < prob)
	{
		BsdfSample sample0;
		sample0.inputs = in;
		m_optics0->calcBsdfSample(sample0.inputs, sample0.outputs, sidedness);

		BsdfEvaluation eval1;
		eval1.inputs.set(in.X, sample0.outputs.L, in.V, in.elemental, in.transported);
		m_optics1->calcBsdf(eval1.inputs, eval1.outputs, sidedness);

		BsdfPdfQuery query0, query1;
		query0.inputs.set(sample0);
		query1.inputs.set(eval1);
		m_optics0->calcBsdfSamplePdfW(query0.inputs, query0.outputs, sidedness);
		m_optics1->calcBsdfSamplePdfW(query1.inputs, query1.outputs, sidedness);

		const SpectralStrength bsdf0 = sample0.outputs.pdfAppliedBsdf * query0.outputs.sampleDirPdfW;
		const SpectralStrength bsdf = bsdf0 * ratio + eval1.outputs.bsdf * (SpectralStrength(1) - ratio);
		const real             pdfW = query0.outputs.sampleDirPdfW * prob + query1.outputs.sampleDirPdfW * (1.0_r - prob);
		out.pdfAppliedBsdf = bsdf / pdfW;
		out.L = sample0.outputs.L;
	}
	else
	{
		BsdfSample sample1;
		sample1.inputs = in;
		m_optics1->calcBsdfSample(sample1.inputs, sample1.outputs, sidedness);

		BsdfEvaluation eval0;
		eval0.inputs.set(in.X, sample1.outputs.L, in.V, in.elemental, in.transported);
		m_optics0->calcBsdf(eval0.inputs, eval0.outputs, sidedness);

		BsdfPdfQuery query0, query1;
		query1.inputs.set(sample1);
		query0.inputs.set(eval0);
		m_optics1->calcBsdfSamplePdfW(query1.inputs, query1.outputs, sidedness);
		m_optics0->calcBsdfSamplePdfW(query0.inputs, query0.outputs, sidedness);

		const SpectralStrength bsdf1 = sample1.outputs.pdfAppliedBsdf * query1.outputs.sampleDirPdfW;
		const SpectralStrength bsdf = eval0.outputs.bsdf * ratio + bsdf1 * (SpectralStrength(1) - ratio);
		const real             pdfW = query0.outputs.sampleDirPdfW * prob + query1.outputs.sampleDirPdfW * (1.0_r - prob);
		out.pdfAppliedBsdf = bsdf / pdfW;
		out.L = sample1.outputs.L;
	}
}

void LerpedSurfaceOptics::calcBsdfSamplePdfW(
	const BsdfPdfQuery::Input& in,
	BsdfPdfQuery::Output&      out,
	const SidednessAgreement&  sidedness) const
{
	const SpectralStrength ratio = m_sampler.sample(*m_ratio, in.X);
	const real             prob  = probabilityOfPickingOptics0(ratio);

	BsdfPdfQuery::Output query0, query1;
	m_optics0->calcBsdfSamplePdfW(in, query0, sidedness);
	m_optics1->calcBsdfSamplePdfW(in, query1, sidedness);

	out.sampleDirPdfW = query0.sampleDirPdfW * prob + query1.sampleDirPdfW * (1.0_r - prob);
}

real LerpedSurfaceOptics::probabilityOfPickingOptics0(const SpectralStrength& ratio)
{
	return math::clamp(ratio.calcLuminance(EQuantity::ECF), 0.01_r, 1.0_r);
}

}// end namespace ph