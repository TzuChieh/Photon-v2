#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Engine/Core/SampleGenerator/SampleFlow.h"

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
		calcBsdfCore(
			eval.context,
			eval.inputs,
			eval.outputs);
	}
	else
	{
		// Specifying elemental and phenomena together is redundant and not allowed
		PH_ASSERT(!(
			eval.context.targetPhenomena != ALL_SURFACE_PHENOMENA &&
			eval.context.elemental != ALL_SURFACE_ELEMENTALS));

		eval.outputs.setContributability(false);

		// If not all phenomena is queried, we iterate and accumulate
		for(const auto& ei : getElemenalIteratorProxy(eval.context.targetPhenomena))
		{
			BsdfQueryContext ctx = eval.context;
			ctx.elemental = ei.elemental;
			ctx.targetPhenomena = SurfacePhenomena(ei.phenomenon);

			BsdfEvalOutput outputs;
			calcBsdfCore(
				ctx,
				eval.inputs,
				outputs);

			eval.outputs.add(outputs);
		}
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
		genBsdfSampleCore(
			sample.context,
			sample.inputs,
			sampleFlow,
			sample.outputs);
	}
	else
	{
		// Specifying elemental and phenomena together is redundant and not allowed
		PH_ASSERT(!(
			sample.context.targetPhenomena != ALL_SURFACE_PHENOMENA &&
			sample.context.elemental != ALL_SURFACE_ELEMENTALS));

		sample.outputs.setContributability(false);

		auto ei = beginElementalIteratorFor(sample.context.targetPhenomena);
		auto [optPickedElemental, numPickableElementals] = math::uniform_pick<real, SurfaceElemental>(
			[this, &ei]() -> std::optional<SurfaceElemental>
			{
				if(ei != endElementalIterator())
				{
					return ei->elemental;
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

	calcBsdfPdfCore(
		pdfQuery.context,
		pdfQuery.inputs,
		pdfQuery.outputs);
}

}// end namespace ph
