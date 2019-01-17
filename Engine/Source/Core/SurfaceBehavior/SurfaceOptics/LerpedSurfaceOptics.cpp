#include "Core/SurfaceBehavior/SurfaceOptics/LerpedSurfaceOptics.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Math/Random.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Common/assertion.h"
#include "Core/Texture/TConstantTexture.h"
#include "Math/math.h"

#include <utility>

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
	PH_ASSERT(optics0 && optics1 && ratio);

	m_phenomena.set({});
	m_phenomena.unionWith(m_optics0->m_phenomena);
	m_phenomena.unionWith(m_optics1->m_phenomena);

	m_numElementals = m_optics0->m_numElementals + m_optics1->m_numElementals;
}

ESurfacePhenomenon LerpedSurfaceOptics::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_LT(elemental, m_optics0->m_numElementals + m_optics1->m_numElementals);

	if(elemental < m_optics0->m_numElementals)
	{
		return m_optics0->getPhenomenonOf(elemental);
	}
	else
	{
		const SurfaceElemental localElemental = elemental - m_optics0->m_numElementals;
		return m_optics1->getPhenomenonOf(localElemental);
	}
}

void LerpedSurfaceOptics::calcBsdf(
	const BsdfEvaluation::Input& in,
	BsdfEvaluation::Output&      out,
	const SidednessAgreement&    sidedness) const
{
	const SpectralStrength ratio = m_sampler.sample(*m_ratio, in.X);

	if(in.elemental == ALL_ELEMENTALS)
	{
		BsdfEvaluation::Output eval0, eval1;
		m_optics0->calcBsdf(in, eval0, sidedness);
		m_optics1->calcBsdf(in, eval1, sidedness);

		out.bsdf = eval0.bsdf * ratio + eval1.bsdf * (SpectralStrength(1) - ratio);
	}
	else
	{
		PH_ASSERT(in.elemental < m_numElementals);

		if(in.elemental < m_optics0->m_numElementals)
		{
			m_optics0->calcBsdf(in, out, sidedness);
			out.bsdf.mulLocal(ratio);
		}
		else
		{
			BsdfEvaluation::Input localInput = in;
			localInput.elemental = in.elemental - m_optics0->m_numElementals;
			m_optics1->calcBsdf(localInput, out, sidedness);
			out.bsdf.mulLocal(SpectralStrength(1) - ratio);
		}
	}
}

void LerpedSurfaceOptics::calcBsdfSample(
	const BsdfSample::Input&  in,
	BsdfSample::Output&       out,
	const SidednessAgreement& sidedness) const
{
	const SpectralStrength ratio = m_sampler.sample(*m_ratio, in.X);

	if(in.elemental == ALL_ELEMENTALS)
	{
		const real dart = Random::genUniformReal_i0_e1();

		SpectralStrength sampledRatio  = ratio;
		SurfaceOptics*   sampledOptics = m_optics0.get();
		SurfaceOptics*   anotherOptics = m_optics1.get();
		real             sampledProb   = probabilityOfPickingOptics0(ratio);
		if(dart >= sampledProb)
		{
			sampledRatio = SpectralStrength(1) - sampledRatio;
			std::swap(sampledOptics, anotherOptics);
			sampledProb = 1.0_r - sampledProb;
		}

		BsdfSample::Output sample;
		sampledOptics->calcBsdfSample(in, sample, sidedness);
		if(!sample.isMeasurable())
		{
			out.setMeasurability(false);
			return;
		}

		BsdfEvaluation eval;
		eval.inputs.set(in, sample);
		anotherOptics->calcBsdf(eval.inputs, eval.outputs, sidedness);

		BsdfPdfQuery query[2];
		query[0].inputs.set(in, sample);
		query[1].inputs.set(eval);
		sampledOptics->calcBsdfSamplePdfW(query[0].inputs, query[0].outputs, sidedness);
		anotherOptics->calcBsdfSamplePdfW(query[1].inputs, query[1].outputs, sidedness);

		// TODO: this is quite a harsh condition--it may be possible to just 
		// sample another elemental if one of them has 0 pdfW
		if(query[0].outputs.sampleDirPdfW == 0 || query[1].outputs.sampleDirPdfW == 0)
		{
			out.setMeasurability(false);
			return;
		}

		const SpectralStrength bsdf = 
			sampledRatio * (sample.pdfAppliedBsdf * query[0].outputs.sampleDirPdfW) +
			(SpectralStrength(1) - sampledRatio) * eval.outputs.bsdf;

		const real pdfW = 
			sampledProb * query[0].outputs.sampleDirPdfW +
			(1.0_r - sampledProb) * query[1].outputs.sampleDirPdfW;

		PH_ASSERT_MSG(pdfW > 0 && std::isfinite(pdfW), std::to_string(pdfW));

		out.pdfAppliedBsdf = bsdf / pdfW;
		out.L = sample.L;
		out.setMeasurability(true);
	}
	else
	{
		PH_ASSERT(in.elemental < m_numElementals);

		if(in.elemental < m_optics0->m_numElementals)
		{
			m_optics0->calcBsdfSample(in, out, sidedness);
			out.pdfAppliedBsdf.mulLocal(ratio);
		}
		else
		{
			BsdfSample::Input localInput = in;
			localInput.elemental = in.elemental - m_optics0->m_numElementals;
			m_optics1->calcBsdfSample(localInput, out, sidedness);
			out.pdfAppliedBsdf.mulLocal(SpectralStrength(1) - ratio);
		}
	}
}

void LerpedSurfaceOptics::calcBsdfSamplePdfW(
	const BsdfPdfQuery::Input& in,
	BsdfPdfQuery::Output&      out,
	const SidednessAgreement&  sidedness) const
{
	const SpectralStrength ratio = m_sampler.sample(*m_ratio, in.X);

	if(in.elemental == ALL_ELEMENTALS)
	{
		const real prob  = probabilityOfPickingOptics0(ratio);

		BsdfPdfQuery::Output query0, query1;
		m_optics0->calcBsdfSamplePdfW(in, query0, sidedness);
		m_optics1->calcBsdfSamplePdfW(in, query1, sidedness);

		out.sampleDirPdfW = query0.sampleDirPdfW * prob + query1.sampleDirPdfW * (1.0_r - prob);
	}
	else
	{
		PH_ASSERT(in.elemental < m_numElementals);

		if(in.elemental < m_optics0->m_numElementals)
		{
			m_optics0->calcBsdfSamplePdfW(in, out, sidedness);
		}
		else
		{
			BsdfPdfQuery::Input localInput = in;
			localInput.elemental = in.elemental - m_optics0->m_numElementals;
			m_optics1->calcBsdfSamplePdfW(localInput, out, sidedness);
		}
	}
}

real LerpedSurfaceOptics::probabilityOfPickingOptics0(const SpectralStrength& ratio)
{
	return math::clamp(ratio.calcLuminance(EQuantity::ECF), 0.0_r, 1.0_r);
}

}// end namespace ph