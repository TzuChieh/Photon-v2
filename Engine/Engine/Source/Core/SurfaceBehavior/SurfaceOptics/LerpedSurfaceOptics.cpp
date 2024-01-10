#include "Core/SurfaceBehavior/SurfaceOptics/LerpedSurfaceOptics.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/Texture/TTexture.h"
#include "Core/Texture/constant_textures.h"
#include "Math/math.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>

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
		std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum(ratio)))
{}

LerpedSurfaceOptics::LerpedSurfaceOptics(
	const std::shared_ptr<SurfaceOptics>& optics0,
	const std::shared_ptr<SurfaceOptics>& optics1,
	const std::shared_ptr<TTexture<math::Spectrum>>& ratio) :

	m_optics0(optics0),
	m_optics1(optics1),
	m_ratio  (ratio),
	m_sampler(math::EColorUsage::ECF)
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
	const math::Spectrum ratio = m_sampler.sample(*m_ratio, in.X);

	if(ctx.elemental == ALL_ELEMENTALS)
	{
		BsdfEvalOutput eval0, eval1;
		m_optics0->calcBsdf(ctx, in, eval0);
		m_optics1->calcBsdf(ctx, in, eval1);

		out.bsdf = eval0.bsdf * ratio + eval1.bsdf * (math::Spectrum(1) - ratio);
		out.setMeasurability(out.bsdf);
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
			out.bsdf.mulLocal(math::Spectrum(1) - ratio);
		}
		out.setMeasurability(out.bsdf);
	}
}

void LerpedSurfaceOptics::calcBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             sampleFlow,
	BsdfSampleOutput&       out) const
{
	const math::Spectrum ratio = m_sampler.sample(*m_ratio, in.X);

	if(ctx.elemental == ALL_ELEMENTALS)
	{
		math::Spectrum sampledRatio  = ratio;
		SurfaceOptics* sampledOptics = m_optics0.get();
		SurfaceOptics* anotherOptics = m_optics1.get();
		real           sampledProb   = probabilityOfPickingOptics0(ratio);
		if(!sampleFlow.unflowedPick(sampledProb))
		{
			sampledRatio = math::Spectrum(1) - sampledRatio;
			std::swap(sampledOptics, anotherOptics);
			sampledProb = 1.0_r - sampledProb;
		}

		BsdfSampleOutput sampleOutput;
		sampledOptics->calcBsdfSample(ctx, in, sampleFlow, sampleOutput);
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

		const math::Spectrum bsdf =
			sampledRatio * (sampleOutput.pdfAppliedBsdf * query[0].outputs.sampleDirPdfW) +
			(math::Spectrum(1) - sampledRatio) * eval.outputs.bsdf;

		const real pdfW = 
			sampledProb * query[0].outputs.sampleDirPdfW +
			(1.0_r - sampledProb) * query[1].outputs.sampleDirPdfW;

		PH_ASSERT_MSG(pdfW > 0 && std::isfinite(pdfW), std::to_string(pdfW));

		out.pdfAppliedBsdf = bsdf / pdfW;
		out.L = sampleOutput.L;
	}
	else
	{
		PH_ASSERT(ctx.elemental < m_numElementals);

		if(ctx.elemental < m_optics0->m_numElementals)
		{
			m_optics0->calcBsdfSample(ctx, in, sampleFlow, out);
			out.pdfAppliedBsdf.mulLocal(ratio);
		}
		else
		{
			BsdfQueryContext localCtx = ctx;
			localCtx.elemental = ctx.elemental - m_optics0->m_numElementals;
			m_optics1->calcBsdfSample(localCtx, in, sampleFlow, out);
			out.pdfAppliedBsdf.mulLocal(math::Spectrum(1) - ratio);
		}
	}

	out.setMeasurability(out.pdfAppliedBsdf);
}

void LerpedSurfaceOptics::calcBsdfSamplePdfW(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	const math::Spectrum ratio = m_sampler.sample(*m_ratio, in.X);

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

real LerpedSurfaceOptics::probabilityOfPickingOptics0(const math::Spectrum& ratio)
{
	return math::clamp(ratio.relativeLuminance(math::EColorUsage::ECF), 0.0_r, 1.0_r);
}

}// end namespace ph
