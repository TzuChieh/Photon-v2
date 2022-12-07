#pragma once

#include "DataIO/SDL/TSdlResourceBase.h"
#include "Common/primitive_type.h"
#include "Math/Transform/TDecomposedTransform.h"
#include "Math/Transform/Transform.h"

#include <memory>

namespace ph
{

class Time;

class MotionSource : public TSdlResourceBase<ETypeCategory::Ref_Motion>
{
public:
	MotionSource() = default;
	
	virtual std::unique_ptr<math::Transform> genLocalToWorld(
		const Time& start, 
		const Time& end) const = 0;
};

}// end namespace ph
