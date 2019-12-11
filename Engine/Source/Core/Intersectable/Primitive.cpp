#include "Core/Intersectable/Primitive.h"
#include "Common/assertion.h"
#include "Core/Sample/PositionSample.h"

#include <iostream>

namespace ph
{

Primitive::Primitive(const PrimitiveMetadata* const metadata) :
	m_metadata(metadata)
{
	PH_ASSERT(metadata);
}

void Primitive::genPositionSample(SampleFlow& /* sampleFlow */, PositionSample* const out_sample) const
{
	PH_ASSERT(out_sample);

	out_sample->pdf = 0.0_r;
}

}// end namespace ph
