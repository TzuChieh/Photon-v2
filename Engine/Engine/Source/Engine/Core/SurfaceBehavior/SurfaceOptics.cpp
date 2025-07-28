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
	// In case not all phenomena are queried, we iterate and accumulate
	for(const auto& ei : getElemenalIteratorProxy(ctx.targetPhenomena))
	{
		BsdfQueryContext eCtx = ctx;
		eCtx.elemental = ei.elemental;

		BsdfEvalOutput eOutputs;
		calcElementalBsdf(
			eCtx,
			in,
			eOutputs);

		out.add(eOutputs);
	}
}

void SurfaceOptics::genPhenomenalBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             sampleFlow,
	BsdfSampleOutput&       out) const
{
	// In case not all phenomena are queried, we randomly pick one from the phenomena specified
	auto [optPickedElemental, numPickableElementals] = math::uniform_pick<real, SurfaceElemental>(
		[this, ei = beginElementalIteratorFor(ctx.targetPhenomena)]() mutable -> std::optional<SurfaceElemental>
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
		[&sampleFlow](real probability)
		{
			return sampleFlow.unflowedPick(probability);
		});

	if(!optPickedElemental)
	{
		return;
	}
	PH_ASSERT_GE(numPickableElementals, 1);

	BsdfQueryContext eCtx = ctx;
	eCtx.elemental = *optPickedElemental;

	genElementalBsdfSample(
		eCtx,
		in,
		sampleFlow,
		out);

	if(out)
	{
		const real rcpPickProb = numPickableElementals;
		out.setPdfAppliedBsdfCos(out.getPdfAppliedBsdfCos() * rcpPickProb, out.getCos());
	}
	


	// TODO: sample and weight
	// TODO: how to let PDF know which elemental got picked? is it a good strategy?

	// If not all phenomena is queried, we iterate
	/*for_each_elemental_of(
		sample.context.targetPhenomena,
		*this,
		[this, &sample, &sampleFlow](SurfaceElemental elemental, ESurfacePhenomenon phenomenon)
		{
			BsdfQueryContext ctx = sample.context;
			ctx.elemental = elemental;
			ctx.targetPhenomena = SurfacePhenomena(phenomenon);

			BsdfSampleOutput outputs;
			genBsdfSampleCore(
				ctx,
				sample.inputs,
				sampleFlow,
				outputs);

			eval.outputs.add(outputs);
		});*/
}

void SurfaceOptics::calcPhenomenalBsdfPdf(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
}

uint64 SurfaceOptics::svbsdfHash(const SurfaceHit& X)
{
	// SVBRDF input includes position, 
}

}// end namespace ph
