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
	const std::shared_ptr<SurfaceOptics>& optics1)

	: LerpedSurfaceOptics(
		optics0,
		optics1,
		0.5_r)
{}

LerpedSurfaceOptics::LerpedSurfaceOptics(
	const std::shared_ptr<SurfaceOptics>& optics0,
	const std::shared_ptr<SurfaceOptics>& optics1,
	const real ratio)

	: LerpedSurfaceOptics(
		optics0, 
		optics1, 
		std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum(ratio)))
{}

LerpedSurfaceOptics::LerpedSurfaceOptics(
	const std::shared_ptr<SurfaceOptics>& optics0,
	const std::shared_ptr<SurfaceOptics>& optics1,
	const std::shared_ptr<TTexture<math::Spectrum>>& ratio)

	: m_optics0      (optics0)
	, m_optics1      (optics1)
	, m_ratio        (ratio)
	, m_sampler      (math::EColorUsage::ECF)
	, m_containsDelta(false)
{
	PH_ASSERT(optics0);
	PH_ASSERT(optics1);
	PH_ASSERT(ratio);

	m_phenomena.set({});
	m_phenomena.unionWith(optics0->m_phenomena);
	m_phenomena.unionWith(optics1->m_phenomena);

	m_numElementals = optics0->m_numElementals + optics1->m_numElementals;

	m_containsDelta = 
		optics0->m_phenomena.hasAny(DELTA_SURFACE_PHENOMENA) ||
		optics1->m_phenomena.hasAny(DELTA_SURFACE_PHENOMENA);
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
	if(ctx.elemental == ALL_SURFACE_ELEMENTALS && !m_containsDelta)
	{
		const math::Spectrum ratio = m_sampler.sample(*m_ratio, in.getX());

		BsdfEvalOutput eval0, eval1;
		m_optics0->calcBsdf(ctx, in, eval0);
		m_optics1->calcBsdf(ctx, in, eval1);

		const math::Spectrum bsdf0 = eval0.isMeasurable() ? eval0.getBsdf() : math::Spectrum(0);
		const math::Spectrum bsdf1 = eval1.isMeasurable() ? eval1.getBsdf() : math::Spectrum(0);

		out.setBsdf(bsdf0 * ratio + bsdf1 * (math::Spectrum(1) - ratio));
	}
	else if(ctx.elemental == ALL_SURFACE_ELEMENTALS && m_containsDelta)
	{
		out.setMeasurability(false);
	}
	else
	{
		const math::Spectrum ratio = m_sampler.sample(*m_ratio, in.getX());

		PH_ASSERT(ctx.elemental < m_numElementals);
		if(ctx.elemental < m_optics0->m_numElementals)
		{
			m_optics0->calcBsdf(ctx, in, out);
			
			if(out.isMeasurable())
			{
				out.setBsdf(out.getBsdf() * ratio);
			}
		}
		else
		{
			BsdfQueryContext localCtx = ctx;
			localCtx.elemental = ctx.elemental - m_optics0->m_numElementals;
			m_optics1->calcBsdf(localCtx, in, out);

			if(out.isMeasurable())
			{
				out.setBsdf(out.getBsdf() * (math::Spectrum(1) - ratio));
			}
		}
	}
}

void LerpedSurfaceOptics::genBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             sampleFlow,
	BsdfSampleOutput&       out) const
{
	const math::Spectrum ratio = m_sampler.sample(*m_ratio, in.getX());

	if(ctx.elemental == ALL_SURFACE_ELEMENTALS && !m_containsDelta)
	{
		// When both optics are non-delta, sample the lerped distribution

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
		sampledOptics->genBsdfSample(ctx, in, sampleFlow, sampleOutput);
		if(!sampleOutput.isMeasurable())
		{
			out.setMeasurability(false);
			return;
		}

		BsdfEvalQuery eval;
		eval.inputs.set(in, sampleOutput);
		anotherOptics->calcBsdf(ctx, eval.inputs, eval.outputs);

		const math::Spectrum anotherBsdfCos = eval.outputs.isMeasurable()
			? eval.outputs.getBsdf() * sampleOutput.getCos()
			: math::Spectrum(0);

		BsdfPdfQuery query[2];
		query[0].inputs.set(in, sampleOutput);
		query[1].inputs.set(in, sampleOutput);
		sampledOptics->calcBsdfPdf(ctx, query[0].inputs, query[0].outputs);
		anotherOptics->calcBsdfPdf(ctx, query[1].inputs, query[1].outputs);

		// TODO: this is quite a harsh condition--it may be possible to just 
		// sample another elemental if one of them has 0 pdfW
		if(!query[0].outputs.getSampleDirPdf() || !query[1].outputs.getSampleDirPdf())
		{
			out.setMeasurability(false);
			return;
		}

		const math::Spectrum sampledBsdfCos =
			sampleOutput.getPdfAppliedBsdfCos() * query[0].outputs.getSampleDirPdfW();
		const math::Spectrum bsdfCos =
			sampledRatio * sampledBsdfCos + (math::Spectrum(1) - sampledRatio) * anotherBsdfCos;

		const real pdfW = 
			sampledProb * query[0].outputs.getSampleDirPdfW() +
			(1.0_r - sampledProb) * query[1].outputs.getSampleDirPdfW();
		PH_ASSERT_MSG(pdfW > 0 && std::isfinite(pdfW), std::to_string(pdfW));
		
		out.setPdfAppliedBsdfCos(bsdfCos / pdfW, sampleOutput.getCos());
		out.setL(sampleOutput.getL());
	}
	else if(ctx.elemental == ALL_SURFACE_ELEMENTALS && m_containsDelta)
	{
		// When one or both of the optics are delta, pick one to sample

		math::Spectrum sampledRatio  = ratio;
		SurfaceOptics* sampledOptics = m_optics0.get();
		real           sampledProb   = probabilityOfPickingOptics0(ratio);
		if(!sampleFlow.unflowedPick(sampledProb))
		{
			sampledRatio  = math::Spectrum(1) - sampledRatio;
			sampledOptics = m_optics1.get();
			sampledProb   = 1.0_r - sampledProb;
		}

		sampledOptics->genBsdfSample(ctx, in, sampleFlow, out);
		if(!out.isMeasurable())
		{
			return;
		}

		// Apply the scale (lerp ratio) and account for pick probability
		out.setPdfAppliedBsdfCos(out.getPdfAppliedBsdfCos() * sampledRatio / sampledProb, out.getCos());
	}
	else
	{
		PH_ASSERT(ctx.elemental < m_numElementals);

		if(ctx.elemental < m_optics0->m_numElementals)
		{
			m_optics0->genBsdfSample(ctx, in, sampleFlow, out);

			if(out.isMeasurable())
			{
				out.setPdfAppliedBsdfCos(out.getPdfAppliedBsdfCos() * ratio, out.getCos());
			}
		}
		else
		{
			BsdfQueryContext localCtx = ctx;
			localCtx.elemental = ctx.elemental - m_optics0->m_numElementals;
			m_optics1->genBsdfSample(localCtx, in, sampleFlow, out);

			if(out.isMeasurable())
			{
				out.setPdfAppliedBsdfCos(out.getPdfAppliedBsdfCos() * (math::Spectrum(1) - ratio), out.getCos());
			}
		}
	}
}

void LerpedSurfaceOptics::calcBsdfPdf(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	if(ctx.elemental == ALL_SURFACE_ELEMENTALS && !m_containsDelta)
	{
		const math::Spectrum ratio = m_sampler.sample(*m_ratio, in.getX());
		const real prob = probabilityOfPickingOptics0(ratio);

		BsdfPdfQuery::Output query0, query1;
		m_optics0->calcBsdfPdf(ctx, in, query0);
		m_optics1->calcBsdfPdf(ctx, in, query1);

		const real pdfW0 = query0 ? query0.getSampleDirPdfW() : 0.0_r;
		const real pdfW1 = query1 ? query1.getSampleDirPdfW() : 0.0_r;

		out.setSampleDirPdf(lta::PDF::W(
			pdfW0 * prob + pdfW1 * (1.0_r - prob)));
	}
	else if(ctx.elemental == ALL_SURFACE_ELEMENTALS && m_containsDelta)
	{
		out.setSampleDirPdf({});
	}
	else
	{
		PH_ASSERT(ctx.elemental < m_numElementals);

		if(ctx.elemental < m_optics0->m_numElementals)
		{
			m_optics0->calcBsdfPdf(ctx, in, out);
		}
		else
		{
			BsdfQueryContext localCtx = ctx;
			localCtx.elemental = ctx.elemental - m_optics0->m_numElementals;
			m_optics1->calcBsdfPdf(localCtx, in, out);
		}
	}
}

}// end namespace ph
