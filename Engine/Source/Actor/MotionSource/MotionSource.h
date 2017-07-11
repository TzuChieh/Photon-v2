#pragma once

#include "Common/primitive_type.h"
#include "Math/Transform/TDecomposedTransform.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Math/Transform/Transform.h"

#include <memory>

namespace ph
{

class Time;

class MotionSource : public ISdlResource, public TCommandInterface<MotionSource>
{
public:
	MotionSource() = default;
	virtual ~MotionSource() = 0;
	
	virtual std::unique_ptr<Transform> genLocalToWorld(const Time& start, 
	                                                   const Time& end) const = 0;

// command interface
public:
	MotionSource(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<MotionSource>& targetResource, 
	                            const std::string& functionName, 
	                            const InputPacket& packet);
};

}// end namespace ph