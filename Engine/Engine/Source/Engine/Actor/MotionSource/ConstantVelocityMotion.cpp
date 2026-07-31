#include "Engine/Actor/MotionSource/ConstantVelocityMotion.h"
#include "Engine/Core/Transform/DynamicLinearTranslation.h"
#include "Engine/World/Foundation/CookedMotion.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

namespace ph
{

void ConstantVelocityMotion::storeCooked(
	const CookingContext& ctx,
	CookedMotion& out_motion) const
{
	const math::Vector3R translationT0 = m_velocity.mul(
		ctx.getCommonConfig().getTimeStepStart().getAbsoluteS());
	const math::Vector3R translationT1 = m_velocity.mul(
		ctx.getCommonConfig().getTimeStepEnd().getAbsoluteS());
	const DynamicLinearTranslation translation(translationT0, translationT1);

	out_motion.localToWorld = ctx.getResources().makeTransform<DynamicLinearTranslation>(
		translation);
	out_motion.worldToLocal = ctx.getResources().makeTransform<DynamicLinearTranslation>(
		translation.makeInversed());
}

}// end namespace ph
