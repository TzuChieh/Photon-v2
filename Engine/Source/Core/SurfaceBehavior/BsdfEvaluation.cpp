#include "Core/SurfaceBehavior/BsdfEvaluation.h"
#include "Core/SurfaceBehavior/BsdfSample.h"

namespace ph
{

void BsdfEvaluationInput::set(const BsdfSample& sample)
{
	set(sample.inputs, sample.outputs);
}

void BsdfEvaluationInput::set(const BsdfSampleInput& sampleInput, const BsdfSampleOutput& sampleOutput)
{
	set(
		sampleInput.X,
		sampleOutput.L,
		sampleInput.V,
		sampleInput.elemental,
		sampleInput.transported);
}

}// end namespace ph