#include "Actor/MotionSource/ConstantVelocityMotion.h"
#include "Math/Transform/DynamicLinearTranslation.h"
#include "Core/Quantity/Time.h"
#include "World/Foundation/CookedMotion.h"
#include "World/Foundation/CookingContext.h"
#include "World/Foundation/CookedResourceCollection.h"

namespace ph
{

ConstantVelocityMotion::ConstantVelocityMotion(const math::Vector3R& velocity) :
	MotionSource(),
	m_velocity(velocity)
{}

void ConstantVelocityMotion::storeCooked(
	CookedMotion& out_motion,
	const CookingContext& ctx,
	const MotionCookConfig& config) const
{
	const math::Vector3R translationT0 = m_velocity.mul(config.start.absoluteS);
	const math::Vector3R translationT1 = m_velocity.mul(config.end.absoluteS);
	const math::DynamicLinearTranslation translation(translationT0, translationT1);

	out_motion.localToWorld = ctx.getResources()->makeTransform<math::DynamicLinearTranslation>(
		translation);
	out_motion.worldToLocal = ctx.getResources()->makeTransform<math::DynamicLinearTranslation>(
		translation.makeInversed());
}

std::unique_ptr<math::Transform> ConstantVelocityMotion::genLocalToWorld(
	const Time& start,
	const Time& end) const
{
	const math::Vector3R translationT0 = m_velocity.mul(start.absoluteS);
	const math::Vector3R translationT1 = m_velocity.mul(end.absoluteS);

	return std::make_unique<math::DynamicLinearTranslation>(translationT0, translationT1);
}

}// end namespace ph
