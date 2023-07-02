#include "Actor/Actor.h"
#include "World/Foundation/PreCookReport.h"
#include "World/Foundation/CookOrder.h"

#include <utility>

namespace ph
{

PreCookReport Actor::preCook(CookingContext& ctx) const
{
	return PreCookReport(getId());
}

void Actor::postCook(const CookingContext& ctx, TransientVisualElement& element) const
{}

CookOrder Actor::getCookOrder() const
{
	return CookOrder();
}

}// end namespace ph
