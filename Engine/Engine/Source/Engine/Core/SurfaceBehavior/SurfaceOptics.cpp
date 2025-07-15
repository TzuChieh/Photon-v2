#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Engine/Math/Random/sample.h"

namespace ph
{

namespace
{

inline ElementalInfo next_elemental_of(
	SurfacePhenomena phenomena,
	SurfaceElemental fromElemental,
	const SurfaceOptics& optics)
{
	SurfaceElemental ei = fromElemental;
	while(ei < optics.numElementals())
	{
		const ESurfacePhenomenon phenomenon = optics.getPhenomenonOf(ei);
		if(phenomena.has(phenomenon))
		{
			return {ei, phenomenon};
		}

		++ei;
	}

	return {ei, static_cast<ESurfacePhenomenon>(0)};
}

}// end namespace

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
		for(ElementalInfo ei = next_elemental_of(eval.context.targetPhenomena, 0, *this);
		    ei.elemental < numElementals();
		    ei = next_elemental_of(eval.context.targetPhenomena, ei.elemental + 1, *this))
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

		ElementalInfo ei{};
		math::uniform_pick(
			[this, &sample, &ei]()
			{
				ei = next_elemental_of(sample.context.targetPhenomena, ei.elemental, *this);
				return 
				if(ei.elemental < numElementals())
				{
					return std::optional<SurfaceElemental>(ei.elemental);
				}
				else
				{

				}
				next_elemental_of(sample.context.targetPhenomena, ei.elemental + 1, *this);
			});


		// If not all phenomena is queried, we iterate
		for_each_elemental_of(
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
			});
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
