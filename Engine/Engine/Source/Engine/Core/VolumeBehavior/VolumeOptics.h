#pragma once

#include "Engine/Core/VolumeBehavior/medium_query_fwd.h"
#include "Engine/Math/math_fwd.h"
#include "Engine/Math/Color/Spectrum.h"

#include <Common/primitive_type.h>

#include <memory>

namespace ph
{

class SampleFlow;
class MediumDistanceSampleQuery;

class VolumeOptics
{
public:
	virtual ~VolumeOptics() = 0;

	void genDistanceSample(MediumDistanceSampleQuery& sample, SampleFlow& sampleFlow) const;

private:
	virtual void genDistanceSample(
		const MediumDistanceSampleInput& in,
		SampleFlow& sampleFlow,
		MediumDistanceSampleOutput& out) const = 0;
};

}// end namespace ph
