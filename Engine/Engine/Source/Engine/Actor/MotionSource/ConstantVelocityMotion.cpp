#include "Engine/Actor/MotionSource/ConstantVelocityMotion.h"
#include "Engine/Core/Transform/DynamicLinearTranslation.h"
#include "Engine/Core/Quantity/Time.h"
#include "Engine/World/Foundation/CookedMotion.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

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
	const DynamicLinearTranslation translation(translationT0, translationT1);

	out_motion.localToWorld = ctx.getResources().makeTransform<DynamicLinearTranslation>(
		translation);
	out_motion.worldToLocal = ctx.getResources().makeTransform<DynamicLinearTranslation>(
		translation.makeInversed());
}

std::unique_ptr<Transform> ConstantVelocityMotion::genLocalToWorld(
	const Time& start,
	const Time& end) const
{
	const math::Vector3R translationT0 = m_velocity.mul(start.absoluteS);
	const math::Vector3R translationT1 = m_velocity.mul(end.absoluteS);

	return std::make_unique<DynamicLinearTranslation>(translationT0, translationT1);
}

}// end namespace ph
