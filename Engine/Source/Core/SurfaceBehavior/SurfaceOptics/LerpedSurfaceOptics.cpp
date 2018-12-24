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
		const real prob = probabilityOfPickingOptics0(ratio);
		const real dart = Random::genUniformReal_i0_e1();
		if(dart < prob)
		{
			BsdfSample::Output sample0;
			m_optics0->calcBsdfSample(in, sample0, sidedness);

			BsdfEvaluation eval1;
			eval1.inputs.set(in, sample0);
			m_optics1->calcBsdf(eval1.inputs, eval1.outputs, sidedness);

			BsdfPdfQuery query0, query1;
			query0.inputs.set(in, sample0);
			query1.inputs.set(eval1);
			m_optics0->calcBsdfSamplePdfW(query0.inputs, query0.outputs, sidedness);
			m_optics1->calcBsdfSamplePdfW(query1.inputs, query1.outputs, sidedness);

			const SpectralStrength bsdf0 = sample0.pdfAppliedBsdf * query0.outputs.sampleDirPdfW;
			const SpectralStrength bsdf = bsdf0 * ratio + eval1.outputs.bsdf * (SpectralStrength(1) - ratio);
			const real             pdfW = query0.outputs.sampleDirPdfW * prob + query1.outputs.sampleDirPdfW * (1.0_r - prob);
			out.pdfAppliedBsdf = bsdf / pdfW;
			out.L = sample0.L;
		}
		else
		{
			BsdfSample::Output sample1;
			m_optics1->calcBsdfSample(in, sample1, sidedness);

			BsdfEvaluation eval0;
			eval0.inputs.set(in, sample1);
			m_optics0->calcBsdf(eval0.inputs, eval0.outputs, sidedness);

			BsdfPdfQuery query0, query1;
			query1.inputs.set(in, sample1);
			query0.inputs.set(eval0);
			m_optics1->calcBsdfSamplePdfW(query1.inputs, query1.outputs, sidedness);
			m_optics0->calcBsdfSamplePdfW(query0.inputs, query0.outputs, sidedness);

			const SpectralStrength bsdf1 = sample1.pdfAppliedBsdf * query1.outputs.sampleDirPdfW;
			const SpectralStrength bsdf = eval0.outputs.bsdf * ratio + bsdf1 * (SpectralStrength(1) - ratio);
			const real             pdfW = query0.outputs.sampleDirPdfW * prob + query1.outputs.sampleDirPdfW * (1.0_r - prob);
			out.pdfAppliedBsdf = bsdf / pdfW;
			out.L = sample1.L;
		}

		// FIXME: should check individual optics has valid sample
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
	return math::clamp(ratio.calcLuminance(EQuantity::ECF), 0.01_r, 1.0_r);
}

}// end namespace ph