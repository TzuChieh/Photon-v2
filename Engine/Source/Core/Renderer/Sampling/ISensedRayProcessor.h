#pragma once

#include "Math/math_fwd.h"
#include "Common/primitive_type.h"

namespace ph
{

class Ray;

class ISensedRayProcessor
{
public:
	virtual ~ISensedRayProcessor() = default;

	virtual void process(const math::Vector2D& filmNdc, const Ray& sensedRay) = 0;
	virtual void onBatchStart(uint64 batchNumber);
	virtual void onBatchFinish(uint64 batchNumber);
};

// In-header Implementations:

inline void ISensedRayProcessor::onBatchStart(const uint64 batchNumber)
{}

inline void ISensedRayProcessor::onBatchFinish(const uint64 batchNumber)
{}

}// end namespace ph
