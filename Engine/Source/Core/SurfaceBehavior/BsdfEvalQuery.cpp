#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"

namespace ph
{

void BsdfEvalInput::set(const BsdfSampleQuery& sample)
{
	set(sample.inputs, sample.outputs);
}

void BsdfEvalInput::set(const BsdfSampleInput& sampleInput, const BsdfSampleOutput& sampleOutput)
{
	set(
		sampleInput.X,
		sampleOutput.L,
		sampleInput.V);
}

}// end namespace ph
