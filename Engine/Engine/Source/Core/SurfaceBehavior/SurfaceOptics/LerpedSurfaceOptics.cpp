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

		// One or both of them can fail due to single sided, etc.
		const math::Spectrum bsdf0 = eval0.isMeasurable() ? eval0.getBsdf() : math::Spectrum(0);
		const math::Spectrum bsdf1 = eval1.isMeasurable() ? eval1.getBsdf() : math::Spectrum(0);

		out.setBsdf(bsdf0 * ratio + bsdf1 * (1.0_r - ratio));
	}
	else if(ctx.elemental == ALL_SURFACE_ELEMENTALS && m_containsDelta)
	{
		out.setMeasurability(false);
	}
	else
	{
		const math::Spectrum ratio = m_sampler.sample(*m_ratio, in.getX());

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
			PH_ASSERT_IN_RANGE(ctx.elemental, m_optics0->m_numElementals, m_numElementals);

			BsdfQueryContext localCtx = ctx;
			localCtx.elemental = ctx.elemental - m_optics0->m_numElementals;
			m_optics1->calcBsdf(localCtx, in, out);

			if(out.isMeasurable())
			{
				out.setBsdf(out.getBsdf() * (1.0_r - ratio));
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

	// When both optics are non-delta, sample the lerped distribution
	if(ctx.elemental == ALL_SURFACE_ELEMENTALS && !m_containsDelta)
	{
		SurfaceOptics* sampledOptics = m_optics0.get();
		SurfaceOptics* anotherOptics = m_optics1.get();
		math::Spectrum sampledRatio = ratio;
		real sampledProb = probabilityOfPickingOptics0(ratio);
		if(!sampleFlow.unflowedPick(sampledProb))
		{
			std::swap(sampledOptics, anotherOptics);
			sampledRatio = 1.0_r - sampledRatio;
			sampledProb = 1.0_r - sampledProb;
		}

		BsdfSampleQuery::Output sampleOutputs;
		sampledOptics->genBsdfSample(ctx, in, sampleFlow, sampleOutputs);
		if(!sampleOutputs.isMeasurable())
		{
			out.setMeasurability(false);
			return;
		}

		BsdfEvalQuery eval;
		eval.inputs.set(in, sampleOutputs);
		anotherOptics->calcBsdf(ctx, eval.inputs, eval.outputs);

		const math::Spectrum anotherBsdfCos = eval.outputs.isMeasurable()
			? eval.outputs.getBsdf() * sampleOutputs.getCos() : math::Spectrum(0);

		BsdfPdfQuery sampledPdf, anotherPdf;
		sampledPdf.inputs.set(in, sampleOutputs);
		anotherPdf.inputs.set(in, sampleOutputs);
		sampledOptics->calcBsdfPdf(ctx, sampledPdf.inputs, sampledPdf.outputs);
		anotherOptics->calcBsdfPdf(ctx, anotherPdf.inputs, anotherPdf.outputs);

		// One or both of them can fail due to single sided, etc.
		const real sampledPdfW = sampledPdf.outputs ? sampledPdf.outputs.getSampleDirPdfW() : 0.0_r;
		const real anotherPdfW = anotherPdf.outputs ? anotherPdf.outputs.getSampleDirPdfW() : 0.0_r;
		const real pdfW = sampledProb * sampledPdfW + (1.0_r - sampledProb) * anotherPdfW;

		const math::Spectrum sampledBsdfCos =
			sampleOutputs.getPdfAppliedBsdfCos() * sampledPdfW;
		const math::Spectrum bsdfCos =
			sampledRatio * sampledBsdfCos + (1.0_r - sampledRatio) * anotherBsdfCos;

		out.setPdfAppliedBsdfCos(bsdfCos / pdfW, sampleOutputs.getCos());
		out.setL(sampleOutputs.getL());
	}
	// When one or both of the optics are deltas, pick one to sample
	else if(ctx.elemental == ALL_SURFACE_ELEMENTALS && m_containsDelta)
	{
		SurfaceOptics* sampledOptics = m_optics0.get();
		math::Spectrum sampledRatio = ratio;
		real sampledProb = probabilityOfPickingOptics0(ratio);
		if(!sampleFlow.unflowedPick(sampledProb))
		{
			sampledOptics = m_optics1.get();
			sampledRatio = 1.0_r - sampledRatio;
			sampledProb = 1.0_r - sampledProb;
		}

		sampledOptics->genBsdfSample(ctx, in, sampleFlow, out);
		if(!out.isMeasurable())
		{
			return;
		}

		// Apply the scale (lerp ratio) and account for pick probability
		out.setPdfAppliedBsdfCos(out.getPdfAppliedBsdfCos() * sampledRatio / sampledProb, out.getCos());
	}
	// A specific elemental is requested (only single optics is participating)
	else
	{
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
			PH_ASSERT_IN_RANGE(ctx.elemental, m_optics0->m_numElementals, m_numElementals);

			BsdfQueryContext localCtx = ctx;
			localCtx.elemental = ctx.elemental - m_optics0->m_numElementals;
			m_optics1->genBsdfSample(localCtx, in, sampleFlow, out);

			if(out.isMeasurable())
			{
				out.setPdfAppliedBsdfCos(out.getPdfAppliedBsdfCos() * (1.0_r - ratio), out.getCos());
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
		const real prob0 = probabilityOfPickingOptics0(ratio);

		BsdfPdfQuery::Output query0, query1;
		m_optics0->calcBsdfPdf(ctx, in, query0);
		m_optics1->calcBsdfPdf(ctx, in, query1);

		// One or both of them can fail due to single sided, etc.
		const real pdfW0 = query0 ? query0.getSampleDirPdfW() : 0.0_r;
		const real pdfW1 = query1 ? query1.getSampleDirPdfW() : 0.0_r;

		out.setSampleDirPdf(lta::PDF::W(
			pdfW0 * prob0 + pdfW1 * (1.0_r - prob0)));
	}
	else if(ctx.elemental == ALL_SURFACE_ELEMENTALS && m_containsDelta)
	{
		out.setSampleDirPdf({});
	}
	else
	{
		if(ctx.elemental < m_optics0->m_numElementals)
		{
			m_optics0->calcBsdfPdf(ctx, in, out);
		}
		else
		{
			PH_ASSERT_IN_RANGE(ctx.elemental, m_optics0->m_numElementals, m_numElementals);

			BsdfQueryContext localCtx = ctx;
			localCtx.elemental = ctx.elemental - m_optics0->m_numElementals;
			m_optics1->calcBsdfPdf(localCtx, in, out);
		}
	}
}

}// end namespace ph
