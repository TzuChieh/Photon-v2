#pragma once

#include "Common/primitive_type.h"
#include "Math/Transform/TDecomposedTransform.h"
#include "DataIO/SDL/TCommandInterface.h"
#include "Math/Transform/Transform.h"

#include <memory>

namespace ph
{

class Time;

class MotionSource : public TCommandInterface<MotionSource>
{
public:
	static constexpr ETypeCategory CATEGORY = ETypeCategory::REF_MOTION;

public:
	MotionSource() = default;
	
	virtual std::unique_ptr<math::Transform> genLocalToWorld(
		const Time& start, 
		const Time& end) const = 0;

	ETypeCategory getCategory() const override;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

inline ETypeCategory MotionSource::getCategory() const
{
	return CATEGORY;
}

}// end namespace ph
