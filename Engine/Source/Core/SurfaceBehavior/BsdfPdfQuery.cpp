#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"
#include "Core/SurfaceBehavior/BsdfSample.h"

namespace ph
{

void BsdfPdfQueryInput::set(const BsdfEvaluation& bsdfEval)
{
	set(
		bsdfEval.inputs.X, 
		bsdfEval.inputs.L, 
		bsdfEval.inputs.V, 
		bsdfEval.inputs.elemental);
}

void BsdfPdfQueryInput::set(const BsdfSample& sample)
{
	set(
		sample.inputs.X, 
		sample.outputs.L, 
		sample.inputs.V,
		sample.inputs.elemental);
}

void BsdfPdfQueryInput::set(const BsdfSampleInput& sampleInput, const BsdfSampleOutput& sampleOutput)
{
	set(
		sampleInput.X,
		sampleOutput.L,
		sampleInput.V,
		sampleInput.elemental);
}

}// end namespace ph