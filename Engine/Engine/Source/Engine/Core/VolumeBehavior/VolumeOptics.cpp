#include "Engine/Core/VolumeBehavior/VolumeOptics.h"
#include "Engine/Core/VolumeBehavior/MediumDistanceSampleQuery.h"

namespace ph
{

VolumeOptics::~VolumeOptics() = default;

void VolumeOptics::genDistanceSample(MediumDistanceSampleQuery& sample, SampleFlow& sampleFlow) const
{
	genDistanceSampleCore(
		sample.inputs,
		sampleFlow,
		sample.outputs);
}

}// end namespace ph
