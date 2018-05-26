#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"
#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"

namespace ph
{

SurfaceOptics::SurfaceOptics() : 
	m_phenomena()
{}

SurfaceOptics::~SurfaceOptics() = default;

void SurfaceOptics::evalBsdf(BsdfEvaluation& eval) const
{
	evalBsdf(
		eval.inputs.X, eval.inputs.L, eval.inputs.V, 
		&(eval.outputs.bsdf));
}

void SurfaceOptics::genBsdfSample(BsdfSample& sample) const
{
	genBsdfSample(
		sample.inputs.X, sample.inputs.V,
		&(sample.outputs.L), &(sample.outputs.pdfAppliedBsdf));
}

void SurfaceOptics::calcBsdfSamplePdf(BsdfPdfQuery& pdfQuery) const
{
	calcBsdfSamplePdf(
		pdfQuery.inputs.X, pdfQuery.inputs.L, pdfQuery.inputs.V, 
		&(pdfQuery.outputs.sampleDirPdfW));
}

}// end namespace ph