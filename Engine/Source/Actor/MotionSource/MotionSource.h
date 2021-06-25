#pragma once

#include "Common/primitive_type.h"
#include "Math/Transform/TDecomposedTransform.h"
#include "DataIO/SDL/ISdlResource.h"
#include "Math/Transform/Transform.h"

#include <memory>

namespace ph
{

class Time;

class MotionSource : public ISdlResource
{
public:
	static constexpr ETypeCategory CATEGORY = ETypeCategory::REF_MOTION;

public:
	MotionSource() = default;
	
	virtual std::unique_ptr<math::Transform> genLocalToWorld(
		const Time& start, 
		const Time& end) const = 0;

	ETypeCategory getCategory() const override;
};

// In-header Implementations:

inline ETypeCategory MotionSource::getCategory() const
{
	return CATEGORY;
}

}// end namespace ph
