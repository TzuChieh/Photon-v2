#include "Actor/MotionSource/ConstVelocityMotion.h"

namespace ph
{

ConstVelocityMotion::~ConstVelocityMotion() = default;

std::unique_ptr<Transform> ConstVelocityMotion::genLocalToWorld(const Time& start,
                                                                const Time& end) const
{
	// TODO

	return nullptr;
}

ConstVelocityMotion::ConstVelocityMotion(const InputPacket& packet) :
	MotionSource(packet)
{
	// TODO
}

SdlTypeInfo ConstVelocityMotion::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MOTION, "const-velocity");
}

ExitStatus ConstVelocityMotion::ciExecute(const std::shared_ptr<ConstVelocityMotion>& targetResource,
                                          const std::string& functionName,
                                          const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph