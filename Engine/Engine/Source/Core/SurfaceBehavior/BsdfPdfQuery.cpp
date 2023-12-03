#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"

namespace ph
{

void BsdfPdfInput::set(const BsdfEvalQuery& eval)
{
	set(
		eval.inputs.X,
		eval.inputs.L,
		eval.inputs.V);
}

void BsdfPdfInput::set(const BsdfSampleQuery& sample)
{
	set(
		sample.inputs.X, 
		sample.outputs.L, 
		sample.inputs.V);
}

void BsdfPdfInput::set(const BsdfSampleInput& sampleInput, const BsdfSampleOutput& sampleOutput)
{
	set(
		sampleInput.X,
		sampleOutput.L,
		sampleInput.V);
}

}// end namespace ph
