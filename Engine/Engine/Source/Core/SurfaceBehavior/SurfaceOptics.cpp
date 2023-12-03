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
	calcBsdf(
		eval.context,
		eval.inputs,
		eval.outputs);
}

void SurfaceOptics::calcBsdfSample(BsdfSampleQuery& sample, SampleFlow& sampleFlow) const
{
	calcBsdfSample(
		sample.context,
		sample.inputs, 
		sampleFlow,
		sample.outputs);
}

void SurfaceOptics::calcBsdfSamplePdfW(BsdfPdfQuery& pdfQuery) const
{
	calcBsdfSamplePdfW(
		pdfQuery.context,
		pdfQuery.inputs,
		pdfQuery.outputs);
}

}// end namespace ph
