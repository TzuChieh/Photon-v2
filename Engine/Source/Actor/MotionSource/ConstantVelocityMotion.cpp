#include "Actor/MotionSource/ConstantVelocityMotion.h"
#include "Math/Transform/DynamicLinearTranslation.h"
#include "Core/Quantity/Time.h"

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

}// end namespace ph
