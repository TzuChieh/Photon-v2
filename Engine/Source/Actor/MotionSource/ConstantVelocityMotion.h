#pragma once

#include "Actor/MotionSource/MotionSource.h"
#include "Math/TVector3.h"

namespace ph
{

class ConstantVelocityMotion : public MotionSource, public TCommandInterface<ConstantVelocityMotion>
{
public:
	explicit ConstantVelocityMotion(const math::Vector3R& velocity);

	std::unique_ptr<math::Transform> genLocalToWorld(
		const Time& start,
		const Time& end) const override;

private:
	math::Vector3R m_velocity;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<ConstantVelocityMotion> ciLoad(const InputPacket& packet);
};

}// end namespace ph
