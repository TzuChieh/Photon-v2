#include "Engine/Actor/Actor.h"
#include "Engine/World/Foundation/PreCookReport.h"
#include "Engine/World/Foundation/CookOrder.h"

#include <utility>

namespace ph
{

PH_DEFINE_LOG_GROUP(ActorCooking, Actor);

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

void Actor::setIsInstantiableHint(const bool isInstantiable)
{
	m_isInstantiableHint = isInstantiable;
}

bool Actor::isInstantiableHint() const
{
	return m_isInstantiableHint;
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
