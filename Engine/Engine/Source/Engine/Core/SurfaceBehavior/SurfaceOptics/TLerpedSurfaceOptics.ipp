#pragma once

#include "Engine/Core/SurfaceBehavior/SurfaceOptics/TLerpedSurfaceOptics.h"
#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Math/math.h"
#include "Engine/Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>

#include <utility>

// TODO: maybe we should ensure factor being [0, 1] here (should we trust ECF?)

namespace ph
{

namespace
{

inline std::pair<BsdfQueryContext, BsdfQueryContext> make_derived_contexts(const BsdfQueryContext& ctx)
{
	BsdfQueryContext ctx0 = ctx;
	BsdfQueryContext ctx1 = ctx;

	// We need distinct keys in two optics to avoid correlation. Key generation must be
	// consistent across different BSDF methods. Hash targets are randomly chosen.
	ctx0.key = ctx.key.getNext(0xEF6B9D3A);
	ctx1.key = ctx.key.getNext(0x91E5F1FE);

	return {ctx0, ctx1};
}

}// end namespace

template<typename Factor>
TLerpedSurfaceOptics<Factor>::TLerpedSurfaceOptics(
	const SurfaceOptics* optics0,
	const SurfaceOptics* optics1,
	Factor factor)

	: m_optics0      (optics0)
	, m_optics1      (optics1)
	, m_factor       (std::move(factor))
	, m_containsDelta(false)
{
	PH_ASSERT(optics0);
	PH_ASSERT(optics1);

	m_phenomena.clear();
	m_phenomena.unionWith(optics0->getAllPhenomena());
	m_phenomena.unionWith(optics1->getAllPhenomena());

	m_numElementals = optics0->numElementals() + optics1->numElementals();

	m_containsDelta = 
		optics0->getAllPhenomena().hasAny(ESurfacePhenomenon::Delta) ||
		optics1->getAllPhenomena().hasAny(ESurfacePhenomenon::Delta);
}

template<typename Factor>
ESurfacePhenomenon TLerpedSurfaceOptics<Factor>::getPhenomenonOf(
	const SurfaceElemental elemental) const
{
	PH_ASSERT_IN_RANGE(elemental, 0, m_optics0->numElementals() + m_optics1->numElementals());

	if(elemental < m_optics0->numElementals())
	{
		return m_optics0->getPhenomenonOf(elemental);
	}
	else
	{
		const SurfaceElemental localElemental = elemental - m_optics0->numElementals();
		return m_optics1->getPhenomenonOf(localElemental);
	}
}

template<typename Factor>
void TLerpedSurfaceOptics<Factor>::calcElementalBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	if(ctx.elemental == ALL_SURFACE_ELEMENTALS && !m_containsDelta)
	{
		const math::Spectrum ratio = m_factor(in.getX());
		const auto [ctx0, ctx1] = make_derived_contexts(ctx);

		BsdfEvalOutput eval0, eval1;
		m_optics0->calcElementalBsdf(ctx0, in, eval0);
		m_optics1->calcElementalBsdf(ctx1, in, eval1);

		// One or both of them can fail due to single sided, etc.
		const math::Spectrum bsdf0 = eval0.isContributable() ? eval0.getBsdf() : math::Spectrum(0);
		const math::Spectrum bsdf1 = eval1.isContributable() ? eval1.getBsdf() : math::Spectrum(0);

		out.setBsdf(bsdf0 * ratio + bsdf1 * (1.0_r - ratio));
	}
	else if(ctx.elemental == ALL_SURFACE_ELEMENTALS && m_containsDelta)
	{
		out.setContributability(false);
	}
	else
	{
		const math::Spectrum ratio = m_factor(in.getX());

		if(ctx.elemental < m_optics0->numElementals())
		{
			m_optics0->calcElementalBsdf(ctx, in, out);
			
			if(out.isContributable())
			{
				out.setBsdf(out.getBsdf() * ratio);
			}
		}
		else
		{
			PH_ASSERT_IN_RANGE(ctx.elemental, m_optics0->numElementals(), m_numElementals);

			BsdfQueryContext localCtx = ctx;
			localCtx.elemental = ctx.elemental - m_optics0->numElementals();
			m_optics1->calcElementalBsdf(localCtx, in, out);

			if(out.isContributable())
			{
				out.setBsdf(out.getBsdf() * (1.0_r - ratio));
			}
		}
	}
}

template<typename Factor>
void TLerpedSurfaceOptics<Factor>::genElementalBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             sampleFlow,
	BsdfSampleOutput&       out) const
{
	const math::Spectrum ratio = m_factor(in.getX());

	// When both optics are non-delta, sample the lerped distribution
	if(ctx.elemental == ALL_SURFACE_ELEMENTALS && !m_containsDelta)
	{
		const SurfaceOptics* sampledOptics = m_optics0;
		const SurfaceOptics* anotherOptics = m_optics1;
		math::Spectrum sampledRatio = ratio;
		real sampledProb = probabilityOfPickingOptics0(ratio);
		if(!sampleFlow.unflowedPick(sampledProb))
		{
			std::swap(sampledOptics, anotherOptics);
			sampledRatio = 1.0_r - sampledRatio;
			sampledProb = 1.0_r - sampledProb;
		}

		const auto [sampledCtx, anotherCtx] = make_derived_contexts(ctx);

		BsdfSampleQuery::Output sampleOutputs;
		sampledOptics->genElementalBsdfSample(sampledCtx, in, sampleFlow, sampleOutputs);
		if(!sampleOutputs.isContributable())
		{
			out.setContributability(false);
			return;
		}

		BsdfEvalQuery eval;
		eval.inputs.set(in, sampleOutputs);
		anotherOptics->calcElementalBsdf(anotherCtx, eval.inputs, eval.outputs);

		const math::Spectrum anotherBsdfCos = eval.outputs.isContributable()
			? eval.outputs.getBsdf() * sampleOutputs.getCos() : math::Spectrum(0);

		BsdfPdfQuery sampledPdf, anotherPdf;
		sampledPdf.inputs.set(in, sampleOutputs);
		anotherPdf.inputs.set(in, sampleOutputs);
		sampledOptics->calcElementalBsdfPdf(sampledCtx, sampledPdf.inputs, sampledPdf.outputs);
		anotherOptics->calcElementalBsdfPdf(anotherCtx, anotherPdf.inputs, anotherPdf.outputs);

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
		const SurfaceOptics* sampledOptics = m_optics0;
		math::Spectrum sampledRatio = ratio;
		real sampledProb = probabilityOfPickingOptics0(ratio);
		if(!sampleFlow.unflowedPick(sampledProb))
		{
			sampledOptics = m_optics1;
			sampledRatio = 1.0_r - sampledRatio;
			sampledProb = 1.0_r - sampledProb;
		}

		sampledOptics->genElementalBsdfSample(ctx, in, sampleFlow, out);
		if(!out.isContributable())
		{
			return;
		}

		// Apply the scale (lerp ratio) and account for pick probability
		out.setPdfAppliedBsdfCos(out.getPdfAppliedBsdfCos() * sampledRatio / sampledProb, out.getCos());
	}
	// A specific elemental is requested (only single optics is participating)
	else
	{
		if(ctx.elemental < m_optics0->numElementals())
		{
			m_optics0->genElementalBsdfSample(ctx, in, sampleFlow, out);

			if(out.isContributable())
			{
				out.setPdfAppliedBsdfCos(out.getPdfAppliedBsdfCos() * ratio, out.getCos());
			}
		}
		else
		{
			PH_ASSERT_IN_RANGE(ctx.elemental, m_optics0->numElementals(), m_numElementals);

			BsdfQueryContext localCtx = ctx;
			localCtx.elemental = ctx.elemental - m_optics0->numElementals();
			m_optics1->genElementalBsdfSample(localCtx, in, sampleFlow, out);

			if(out.isContributable())
			{
				out.setPdfAppliedBsdfCos(out.getPdfAppliedBsdfCos() * (1.0_r - ratio), out.getCos());
			}
		}
	}
}

template<typename Factor>
void TLerpedSurfaceOptics<Factor>::calcElementalBsdfPdf(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	if(ctx.elemental == ALL_SURFACE_ELEMENTALS && !m_containsDelta)
	{
		const math::Spectrum ratio = m_factor(in.getX());
		const real prob0 = probabilityOfPickingOptics0(ratio);
		const auto [ctx0, ctx1] = make_derived_contexts(ctx);

		BsdfPdfQuery::Output query0, query1;
		m_optics0->calcElementalBsdfPdf(ctx0, in, query0);
		m_optics1->calcElementalBsdfPdf(ctx1, in, query1);

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
		if(ctx.elemental < m_optics0->numElementals())
		{
			m_optics0->calcElementalBsdfPdf(ctx, in, out);
		}
		else
		{
			PH_ASSERT_IN_RANGE(ctx.elemental, m_optics0->numElementals(), m_numElementals);

			BsdfQueryContext localCtx = ctx;
			localCtx.elemental = ctx.elemental - m_optics0->numElementals();
			m_optics1->calcElementalBsdfPdf(localCtx, in, out);
		}
	}
}

}// end namespace ph
