#pragma once

#include "Actor/MotionSource/MotionSource.h"
#include "Math/TVector3.h"

namespace ph
{

class ConstVelocityMotion final : public MotionSource, public TCommandInterface<ConstVelocityMotion>
{
public:
	ConstVelocityMotion(const Vector3R& velocity);
	virtual ~ConstVelocityMotion() override;

	virtual std::unique_ptr<Transform> genLocalToWorld(const Time& start,
	                                                   const Time& end) const override;

private:
	Vector3R m_velocity;

// command interface
public:
	ConstVelocityMotion(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<ConstVelocityMotion>& targetResource,
	                            const std::string& functionName,
	                            const InputPacket& packet);
};

}// end namespace ph