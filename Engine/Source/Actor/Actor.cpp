#include "Actor/Actor.h"
#include "World/Foundation/PreCookReport.h"
#include "World/Foundation/CookOrder.h"

#include <utility>

namespace ph
{

PreCookReport Actor::preCook(CookingContext& ctx)
{
	return PreCookReport(getId());
}

void Actor::postCook(CookingContext& ctx, CookedUnit& cookedUnit)
{}

CookOrder Actor::getCookOrder() const
{
	return CookOrder();
}

}// end namespace ph
