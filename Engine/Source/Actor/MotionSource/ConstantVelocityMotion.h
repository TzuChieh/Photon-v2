#pragma once

#include "Actor/MotionSource/MotionSource.h"
#include "Math/TVector3.h"

namespace ph
{

class ConstantVelocityMotion final : public MotionSource, public TCommandInterface<ConstantVelocityMotion>
{
public:
	ConstantVelocityMotion(const Vector3R& velocity);
	virtual ~ConstantVelocityMotion() override;

	virtual std::unique_ptr<Transform> genLocalToWorld(const Time& start,
	                                                   const Time& end) const override;

private:
	Vector3R m_velocity;

// command interface
public:
	ConstantVelocityMotion(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<ConstantVelocityMotion>& targetResource,
	                            const std::string& functionName,
	                            const InputPacket& packet);
};

}// end namespace ph