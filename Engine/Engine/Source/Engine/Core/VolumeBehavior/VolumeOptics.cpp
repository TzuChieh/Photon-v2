#include "Engine/Core/VolumeBehavior/VolumeOptics.h"
#include "Engine/Core/VolumeBehavior/MediumDistanceSample.h"

namespace ph
{

VolumeOptics::~VolumeOptics() = default;

void VolumeOptics::genDistanceSample(MediumDistanceSample& sample) const
{
	genDistanceSample(
		sample.inputs.X, sample.inputs.L, sample.inputs.maxDist, 
		&(sample.outputs.dist), &(sample.outputs.pdfAppliedWeight));
}

}// end namespace ph
