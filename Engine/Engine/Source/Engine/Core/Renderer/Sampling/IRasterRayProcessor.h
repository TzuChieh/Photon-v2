#pragma once

#include "Engine/Math/math_fwd.h"
#include "Engine/Math/Color/Spectrum.h"

#include <Common/primitive_type.h>

namespace ph
{

class Ray;
class SampleFlow;

class IRasterRayProcessor
{
public:
	virtual ~IRasterRayProcessor() = default;

	virtual void process(
		const math::Vector2D& rasterCoord,
		const Ray&            sensedRay, 
		const math::Spectrum& quantityWeight,
		SampleFlow&           sampleFlow) = 0;

	virtual void onBatchStart(uint64 batchNumber);
	virtual void onBatchFinish(uint64 batchNumber);
};

// In-header Implementations:

inline void IRasterRayProcessor::onBatchStart(const uint64 batchNumber)
{}

inline void IRasterRayProcessor::onBatchFinish(const uint64 batchNumber)
{}

}// end namespace ph
