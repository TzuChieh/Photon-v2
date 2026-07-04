#include "Engine/Actor/MotionSource/MotionSource.h"
#include "Engine/World/Foundation/CookedMotion.h"
#include "Engine/World/Foundation/CookingContext.h"

namespace ph
{

void MotionSource::cook(
	const CookingContext& ctx,
	CookedMotion& out_motion) const
{
	storeCooked(ctx, out_motion);
}

}// end namespace ph
