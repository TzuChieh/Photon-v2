#pragma once

#include "Math/math_fwd.h"
#include "Math/Color/Spectrum.h"

#include <Common/primitive_type.h>

namespace ph
{

class Ray;
class SampleFlow;

class IReceivedRayProcessor
{
public:
	virtual ~IReceivedRayProcessor() = default;

	virtual void process(
		const math::Vector2D& rasterCoord,
		const Ray&            sensedRay, 
		const math::Spectrum& quantityWeight,
		SampleFlow&           sampleFlow) = 0;

	virtual void onBatchStart(uint64 batchNumber);
	virtual void onBatchFinish(uint64 batchNumber);
};

// In-header Implementations:

inline void IReceivedRayProcessor::onBatchStart(const uint64 batchNumber)
{}

inline void IReceivedRayProcessor::onBatchFinish(const uint64 batchNumber)
{}

}// end namespace ph
