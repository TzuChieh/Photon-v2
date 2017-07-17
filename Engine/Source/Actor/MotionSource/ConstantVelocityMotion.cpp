#include "Actor/MotionSource/ConstantVelocityMotion.h"
#include "Math/Transform/DynamicLinearTranslation.h"
#include "Core/Quantity/Time.h"
#include "FileIO/InputPacket.h"

namespace ph
{

ConstantVelocityMotion::ConstantVelocityMotion(const Vector3R& velocity) :
	MotionSource(),
	m_velocity(velocity)
{

}

ConstantVelocityMotion::~ConstantVelocityMotion() = default;

std::unique_ptr<Transform> ConstantVelocityMotion::genLocalToWorld(const Time& start,
                                                                   const Time& end) const
{
	const Vector3R translationT0 = m_velocity.mul(start.absoluteS);
	const Vector3R translationT1 = m_velocity.mul(end.absoluteS);

	return std::make_unique<DynamicLinearTranslation>(translationT0, translationT1);
}

ConstantVelocityMotion::ConstantVelocityMotion(const InputPacket& packet) :
	MotionSource(packet)
{
	m_velocity = packet.getVector3r("velocity", Vector3R(0), DataTreatment::REQUIRED());
}

SdlTypeInfo ConstantVelocityMotion::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MOTION, "constant-velocity");
}

ExitStatus ConstantVelocityMotion::ciExecute(const std::shared_ptr<ConstantVelocityMotion>& targetResource,
                                             const std::string& functionName,
                                             const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph