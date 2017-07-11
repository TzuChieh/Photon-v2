#pragma once

#include "Actor/MotionSource/MotionSource.h"

namespace ph
{

class ConstVelocityMotion final : public MotionSource, public TCommandInterface<ConstVelocityMotion>
{
public:
	virtual ~ConstVelocityMotion() override;

	virtual std::unique_ptr<Transform> genLocalToWorld(const Time& start,
	                                                   const Time& end) const override;

// command interface
public:
	ConstVelocityMotion(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<ConstVelocityMotion>& targetResource,
	                            const std::string& functionName,
	                            const InputPacket& packet);
};

}// end namespace ph