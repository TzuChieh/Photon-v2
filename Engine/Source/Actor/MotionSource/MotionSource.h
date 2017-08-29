#pragma once

#include "Common/primitive_type.h"
#include "Math/Transform/TDecomposedTransform.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Math/Transform/Transform.h"

#include <memory>

namespace ph
{

class Time;

class MotionSource : public TCommandInterface<MotionSource>
{
public:
	MotionSource() = default;
	virtual ~MotionSource() = 0;
	
	virtual std::unique_ptr<Transform> genLocalToWorld(const Time& start, 
	                                                   const Time& end) const = 0;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph