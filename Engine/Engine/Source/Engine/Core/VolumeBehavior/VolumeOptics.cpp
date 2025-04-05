#include "Engine/Core/VolumeBehavior/VolumeOptics.h"
#include "Engine/Core/VolumeBehavior/VolumeDistanceSample.h"

namespace ph
{

VolumeOptics::~VolumeOptics() = default;

void VolumeOptics::sample(VolumeDistanceSample& sample) const
{
	sampleDistance(
		sample.inputs.X, sample.inputs.L, sample.inputs.maxDist, 
		&(sample.outputs.dist), &(sample.outputs.pdfAppliedWeight));
}

}// end namespace ph
