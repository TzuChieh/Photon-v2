#include "Core/SurfaceBehavior/BSDF.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"
#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"

namespace ph
{

BSDF::~BSDF() = default;

void BSDF::evaluate(BsdfEvaluation& eval) const
{
	evaluate(eval.inputs.X, eval.inputs.L, eval.inputs.V, 
	         &(eval.outputs.bsdf), &(eval.outputs.phenomenon));
}

void BSDF::sample(BsdfSample& sample) const
{
	genSample(sample.inputs.X, sample.inputs.V, 
	          &(sample.outputs.L), &(sample.outputs.pdfAppliedBsdf), &(sample.outputs.phenomenon));
}

void BSDF::calcPdf(BsdfPdfQuery& pdfQuery) const
{
	calcSampleDirPdfW(pdfQuery.inputs.X, pdfQuery.inputs.L, pdfQuery.inputs.V, pdfQuery.inputs.phenomenon, 
	                  &(pdfQuery.outputs.sampleDirPdfW));
}

}// end namespace ph