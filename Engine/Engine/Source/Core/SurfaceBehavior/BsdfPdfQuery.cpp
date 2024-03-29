#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"

namespace ph
{

void BsdfPdfInput::set(const BsdfEvalInput& evalInput)
{
	set(
		evalInput.getX(),
		evalInput.getL(),
		evalInput.getV());
}

void BsdfPdfInput::set(const BsdfSampleQuery& sample)
{
	set(
		sample.inputs.getX(),
		sample.outputs.getL(),
		sample.inputs.getV());
}

void BsdfPdfInput::set(const BsdfSampleInput& sampleInput, const BsdfSampleOutput& sampleOutput)
{
	set(
		sampleInput.getX(),
		sampleOutput.getL(),
		sampleInput.getV());
}

}// end namespace ph
