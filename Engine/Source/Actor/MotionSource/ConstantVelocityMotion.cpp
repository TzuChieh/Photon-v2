#include "Actor/MotionSource/ConstantVelocityMotion.h"
#include "Math/Transform/DynamicLinearTranslation.h"
#include "Core/Quantity/Time.h"
#include "DataIO/SDL/InputPacket.h"

namespace ph
{

ConstantVelocityMotion::ConstantVelocityMotion(const math::Vector3R& velocity) :
	MotionSource(),
	m_velocity(velocity)
{}

std::unique_ptr<math::Transform> ConstantVelocityMotion::genLocalToWorld(
	const Time& start,
	const Time& end) const
{
	const math::Vector3R translationT0 = m_velocity.mul(start.absoluteS);
	const math::Vector3R translationT1 = m_velocity.mul(end.absoluteS);

	return std::make_unique<math::DynamicLinearTranslation>(translationT0, translationT1);
}

SdlTypeInfo ConstantVelocityMotion::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MOTION, "constant-velocity");
}

void ConstantVelocityMotion::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<ConstantVelocityMotion>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<ConstantVelocityMotion> ConstantVelocityMotion::ciLoad(const InputPacket& packet)
{
	const auto velocity = packet.getVector3("velocity", 
		math::Vector3R(0), DataTreatment::REQUIRED());

	return std::make_unique<ConstantVelocityMotion>(velocity);
}

}// end namespace ph
