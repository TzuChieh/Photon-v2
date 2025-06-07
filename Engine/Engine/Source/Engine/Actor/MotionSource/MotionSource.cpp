#include "Engine/Actor/MotionSource/MotionSource.h"
#include "Engine/World/Foundation/CookedMotion.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/World/Foundation/CookingContext.h"

#include <Common/assertion.h>

namespace ph
{

CookedMotion* MotionSource::createCooked(
	const CookingContext& ctx,
	const MotionCookConfig& config) const
{
	CookedMotion* cookedMotion = ctx.getResources().makeMotion(getId());
	PH_ASSERT(cookedMotion);

	storeCooked(*cookedMotion, ctx, config);

	return cookedMotion;
}

}// end namespace ph
