#include "Actor/Actor.h"
#include "World/Foundation/PreCookReport.h"
#include "World/Foundation/CookOrder.h"

#include <utility>

namespace ph
{

PreCookReport Actor::preCook(const CookingContext& ctx) const
{
	return PreCookReport(getId());
}

void Actor::postCook(const CookingContext& ctx, TransientVisualElement& element) const
{}

CookOrder Actor::getCookOrder() const
{
	return CookOrder();
}

TransientVisualElement Actor::stagelessCook(const CookingContext& ctx) const
{
	PreCookReport report = preCook(ctx);
	if(!report.isCookable())
	{
		return {};
	}

	TransientVisualElement visualElement = cook(ctx, report);
	postCook(ctx, visualElement);

	return visualElement;
}

}// end namespace ph
