#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"

#include <utility>

namespace ph
{

SurfaceOptics::SurfaceOptics() : 
	m_phenomena(),
	m_numElementals(1)
{}

SurfaceOptics::~SurfaceOptics() = default;

void SurfaceOptics::calcBsdf(BsdfEvalQuery& eval) const
{
	if(!eval.context.sidedness.isSidednessAgreed(eval.inputs.getX(), eval.inputs.getL()) ||
	   !eval.context.sidedness.isSidednessAgreed(eval.inputs.getX(), eval.inputs.getV()))
	{
		eval.outputs.setMeasurability(false);
		return;
	}

	calcBsdf(
		eval.context,
		eval.inputs,
		eval.outputs);
}

void SurfaceOptics::calcBsdfSample(BsdfSampleQuery& sample, SampleFlow& sampleFlow) const
{
	if(!sample.context.sidedness.isSidednessAgreed(sample.inputs.getX(), sample.inputs.getV()))
	{
		sample.outputs.setMeasurability(false);
		return;
	}

	calcBsdfSample(
		sample.context,
		sample.inputs, 
		sampleFlow,
		sample.outputs);

	if(sample.outputs.isMeasurable() &&
	   !sample.context.sidedness.isSidednessAgreed(sample.inputs.getX(), sample.outputs.getL()))
	{
		sample.outputs.setMeasurability(false);
		return;
	}
}

void SurfaceOptics::calcBsdfSamplePdfW(BsdfPdfQuery& pdfQuery) const
{
	if(!pdfQuery.context.sidedness.isSidednessAgreed(pdfQuery.inputs.getX(), pdfQuery.inputs.getL()) ||
	   !pdfQuery.context.sidedness.isSidednessAgreed(pdfQuery.inputs.getX(), pdfQuery.inputs.getV()))
	{
		pdfQuery.outputs.setSampleDirPdfW(0);
		return;
	}

	calcBsdfSamplePdfW(
		pdfQuery.context,
		pdfQuery.inputs,
		pdfQuery.outputs);
}

}// end namespace ph
