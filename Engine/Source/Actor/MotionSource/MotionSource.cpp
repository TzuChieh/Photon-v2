#include "Actor/MotionSource/MotionSource.h"
#include "World/Foundation/CookedMotion.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "World/Foundation/CookingContext.h"
#include "Common/assertion.h"

namespace ph
{

CookedMotion* MotionSource::genCooked(
	const CookingContext& ctx,
	const MotionCookConfig& config) const
{
	CookedMotion* cookedMotion = ctx.getResources()->makeMotion(getId());
	PH_ASSERT(cookedMotion);

	cook(*cookedMotion, ctx, config);

	return cookedMotion;
}

}// end namespace ph
