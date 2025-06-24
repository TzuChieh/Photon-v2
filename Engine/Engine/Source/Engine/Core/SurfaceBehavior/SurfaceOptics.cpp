#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfPdfQuery.h"

#include <utility>

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

	calcBsdfCore(
		eval.context,
		eval.inputs,
		eval.outputs);
}

void SurfaceOptics::genBsdfSample(BsdfSampleQuery& sample, SampleFlow& sampleFlow) const
{
	if(!sample.context.sidedness.isSidednessAgreed(sample.inputs.getX(), sample.inputs.getV()))
	{
		sample.outputs.setContributability(false);
		return;
	}

	genBsdfSampleCore(
		sample.context,
		sample.inputs, 
		sampleFlow,
		sample.outputs);

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
