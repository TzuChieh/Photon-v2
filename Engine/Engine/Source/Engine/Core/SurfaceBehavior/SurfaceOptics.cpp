#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Engine/Core/SampleGenerator/SampleFlow.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Math/hash.h"

namespace ph
{

SurfaceOptics::SurfaceOptics() : 
	m_phenomena(),
	m_numElementals(1)
{}

void SurfaceOptics::calcBsdf(BsdfEvalQuery& eval) const
{
	if(!eval.context.sidedness.isSidednessAgreed(eval.inputs.getX(), eval.inputs.getL()) ||
	   !eval.context.sidedness.isSidednessAgreed(eval.inputs.getX(), eval.inputs.getV()))
	{
		eval.outputs.setContributability(false);
		return;
	}

	if(eval.context.targetPhenomena == ALL_SURFACE_PHENOMENA)
	{
		calcElementalBsdf(
			eval.context,
			eval.inputs,
			eval.outputs);
	}
	else
	{
		calcPhenomenalBsdf(
			eval.context,
			eval.inputs,
			eval.outputs);
	}
}

void SurfaceOptics::genBsdfSample(BsdfSampleQuery& sample, SampleFlow& sampleFlow) const
{
	if(!sample.context.sidedness.isSidednessAgreed(sample.inputs.getX(), sample.inputs.getV()))
	{
		sample.outputs.setContributability(false);
		return;
	}

	if(sample.context.targetPhenomena == ALL_SURFACE_PHENOMENA)
	{
		genElementalBsdfSample(
			sample.context,
			sample.inputs,
			sampleFlow,
			sample.outputs);
	}
	else
	{
		
	}

	if(sample.outputs.isContributable() &&
	   !sample.context.sidedness.isSidednessAgreed(sample.inputs.getX(), sample.outputs.getL()))
	{
		sample.outputs.setContributability(false);
		return;
	}
}

void SurfaceOptics::calcBsdfPdf(BsdfPdfQuery& pdfQuery) const
{
	if(!pdfQuery.context.sidedness.isSidednessAgreed(pdfQuery.inputs.getX(), pdfQuery.inputs.getL()) ||
	   !pdfQuery.context.sidedness.isSidednessAgreed(pdfQuery.inputs.getX(), pdfQuery.inputs.getV()))
	{
		pdfQuery.outputs.setSampleDirPdf({});
		return;
	}

	calcElementalBsdfPdf(
		pdfQuery.context,
		pdfQuery.inputs,
		pdfQuery.outputs);
}

void SurfaceOptics::calcPhenomenalBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	PH_ASSERT(ctx.targetPhenomena != ALL_SURFACE_PHENOMENA);

	real pdf;
	const auto optElemental = selectElementalFromKey(ctx, in.getBase(), &pdf);
	if(!optElemental)
	{
		return;
	}

	BsdfQueryContext eCtx = ctx;
	eCtx.elemental = *optElemental;

	// Update key as we consumed one for selecting elemental
	BsdfInputBase eBase;
	eBase.set(in.getBase().getKey().getNext());

	BsdfEvalInput eIn;
	eIn.set(eBase, in.getX(), in.getL(), in.getV());

	calcElementalBsdf(
		eCtx,
		eIn,
		out);

	if(out)
	{
		const real rcpPdf = 1.0_r / pdf;
		out.setBsdf(out.getBsdf() * rcpPdf);
	}
}

void SurfaceOptics::genPhenomenalBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             sampleFlow,
	BsdfSampleOutput&       out) const
{
	PH_ASSERT(ctx.targetPhenomena != ALL_SURFACE_PHENOMENA);

	real pdf;
	const auto optElemental = selectElementalFromKey(ctx, in.getBase(), &pdf);
	if(!optElemental)
	{
		return;
	}

	BsdfQueryContext eCtx = ctx;
	eCtx.elemental = *optElemental;

	// Update key as we consumed one for selecting elemental
	BsdfInputBase eBase;
	eBase.set(in.getBase().getKey().getNext());

	BsdfSampleInput eIn;
	eIn.set(eBase, in.getX(), in.getV());

	genElementalBsdfSample(
		eCtx,
		eIn,
		sampleFlow,
		out);

	if(out)
	{
		const real rcpPdf = 1.0_r / pdf;
		out.setPdfAppliedBsdfCos(out.getPdfAppliedBsdfCos() * rcpPdf, out.getCos());
	}
}

void SurfaceOptics::calcPhenomenalBsdfPdf(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	PH_ASSERT(ctx.targetPhenomena != ALL_SURFACE_PHENOMENA);

	real pdf;
	const auto optElemental = selectElementalFromKey(ctx, in.getBase(), &pdf);
	if(!optElemental)
	{
		return;
	}

	BsdfQueryContext eCtx = ctx;
	eCtx.elemental = *optElemental;

	// Update key as we consumed one for selecting elemental
	BsdfInputBase eBase;
	eBase.set(in.getBase().getKey().getNext());

	BsdfPdfInput eIn;
	eIn.set(eBase, in.getX(), in.getL(), in.getV());

	calcElementalBsdfPdf(
		eCtx,
		eIn,
		out);

	if(out)
	{
		const real rcpPdf = 1.0_r / pdf;
		out.setSampleDirPdf(out.getSampleDirPdf() * rcpPdf);
	}
}

std::optional<SurfaceElemental> SurfaceOptics::selectElementalFromKey(
	const BsdfQueryContext& ctx,
	const BsdfInputBase& in,
	real* const out_pdf) const
{
	// In case not all phenomena are queried, we randomly pick one from the phenomena specified
	SurfaceElemental numPickableElementals;
	auto optPickedElemental = math::uniform_pick<real, SurfaceElemental>(
		[this, ei = beginElementalIteratorFor(ctx.targetPhenomena)]() mutable
		-> std::optional<SurfaceElemental>
		{
			if(ei != endElementalIterator())
			{
				return (ei++)->elemental;
			}
			else
			{
				return std::nullopt;
			}
		},
		[sample = in.getKey().getValueAsSample()](real probability) mutable
		{
			return math::reused_pick(probability, sample);
		},
		&numPickableElementals);

	if(!optPickedElemental)
	{
		return std::nullopt;
	}
	PH_ASSERT_GE(numPickableElementals, 1);

	PH_ASSERT(out_pdf);
	*out_pdf = 1.0_r / numPickableElementals;
	return optPickedElemental;
}

}// end namespace ph
