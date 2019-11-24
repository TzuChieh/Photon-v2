#include "Core/SurfaceBehavior/SurfaceOptics/LerpedSurfaceOptics.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Math/Random.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Common/assertion.h"
#include "Core/Texture/TConstantTexture.h"
#include "Math/math.h"

#include <utility>

// TODO: maybe we should ensure factor being [0, 1] here (should we trust ECF?)

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
	m_ratio  (ratio),
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
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	const SpectralStrength ratio = m_sampler.sample(*m_ratio, in.X);

	if(ctx.elemental == ALL_ELEMENTALS)
	{
		BsdfEvalOutput eval0, eval1;
		m_optics0->calcBsdf(ctx, in, eval0);
		m_optics1->calcBsdf(ctx, in, eval1);

		out.bsdf = eval0.bsdf * ratio + eval1.bsdf * (SpectralStrength(1) - ratio);
	}
	else
	{
		PH_ASSERT(ctx.elemental < m_numElementals);

		if(ctx.elemental < m_optics0->m_numElementals)
		{
			m_optics0->calcBsdf(ctx, in, out);
			out.bsdf.mulLocal(ratio);
		}
		else
		{
			BsdfQueryContext localCtx = ctx;
			localCtx.elemental = ctx.elemental - m_optics0->m_numElementals;
			m_optics1->calcBsdf(localCtx, in, out);
			out.bsdf.mulLocal(SpectralStrength(1) - ratio);
		}
	}
}

void LerpedSurfaceOptics::calcBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	BsdfSampleOutput&       out) const
{
	const SpectralStrength ratio = m_sampler.sample(*m_ratio, in.X);

	if(ctx.elemental == ALL_ELEMENTALS)
	{
		const real dart = math::Random::genUniformReal_i0_e1();

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

		BsdfSampleOutput sampleOutput;
		sampledOptics->calcBsdfSample(ctx, in, sampleOutput);
		if(!sampleOutput.isMeasurable())
		{
			out.setMeasurability(false);
			return;
		}

		BsdfEvalQuery eval;
		eval.inputs.set(in, sampleOutput);
		anotherOptics->calcBsdf(ctx, eval.inputs, eval.outputs);

		BsdfPdfQuery query[2];
		query[0].inputs.set(in, sampleOutput);
		query[1].inputs.set(eval);
		sampledOptics->calcBsdfSamplePdfW(ctx, query[0].inputs, query[0].outputs);
		anotherOptics->calcBsdfSamplePdfW(ctx, query[1].inputs, query[1].outputs);

		// TODO: this is quite a harsh condition--it may be possible to just 
		// sample another elemental if one of them has 0 pdfW
		if(query[0].outputs.sampleDirPdfW == 0 || query[1].outputs.sampleDirPdfW == 0)
		{
			out.setMeasurability(false);
			return;
		}

		const SpectralStrength bsdf = 
			sampledRatio * (sampleOutput.pdfAppliedBsdf * query[0].outputs.sampleDirPdfW) +
			(SpectralStrength(1) - sampledRatio) * eval.outputs.bsdf;

		const real pdfW = 
			sampledProb * query[0].outputs.sampleDirPdfW +
			(1.0_r - sampledProb) * query[1].outputs.sampleDirPdfW;

		PH_ASSERT_MSG(pdfW > 0 && std::isfinite(pdfW), std::to_string(pdfW));

		out.pdfAppliedBsdf = bsdf / pdfW;
		out.L = sampleOutput.L;
		out.setMeasurability(true);
	}
	else
	{
		PH_ASSERT(ctx.elemental < m_numElementals);

		if(ctx.elemental < m_optics0->m_numElementals)
		{
			m_optics0->calcBsdfSample(ctx, in, out);
			out.pdfAppliedBsdf.mulLocal(ratio);
		}
		else
		{
			BsdfQueryContext localCtx = ctx;
			localCtx.elemental = ctx.elemental - m_optics0->m_numElementals;
			m_optics1->calcBsdfSample(localCtx, in, out);
			out.pdfAppliedBsdf.mulLocal(SpectralStrength(1) - ratio);
		}
	}
}

void LerpedSurfaceOptics::calcBsdfSamplePdfW(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	const SpectralStrength ratio = m_sampler.sample(*m_ratio, in.X);

	if(ctx.elemental == ALL_ELEMENTALS)
	{
		const real prob = probabilityOfPickingOptics0(ratio);

		BsdfPdfQuery::Output query0, query1;
		m_optics0->calcBsdfSamplePdfW(ctx, in, query0);
		m_optics1->calcBsdfSamplePdfW(ctx, in, query1);

		out.sampleDirPdfW = query0.sampleDirPdfW * prob + query1.sampleDirPdfW * (1.0_r - prob);
	}
	else
	{
		PH_ASSERT(ctx.elemental < m_numElementals);

		if(ctx.elemental < m_optics0->m_numElementals)
		{
			m_optics0->calcBsdfSamplePdfW(ctx, in, out);
		}
		else
		{
			BsdfQueryContext localCtx = ctx;
			localCtx.elemental = ctx.elemental - m_optics0->m_numElementals;
			m_optics1->calcBsdfSamplePdfW(localCtx, in, out);
		}
	}
}

real LerpedSurfaceOptics::probabilityOfPickingOptics0(const SpectralStrength& ratio)
{
	return math::clamp(ratio.calcLuminance(EQuantity::ECF), 0.0_r, 1.0_r);
}

}// end namespace ph
