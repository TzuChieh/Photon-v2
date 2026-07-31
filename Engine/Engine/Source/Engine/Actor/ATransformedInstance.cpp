#include "Engine/Actor/ATransformedInstance.h"
#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/Core/Intersection/IntersectableBuilder.h"
#include "Engine/Core/Transform/StaticAffineTransform.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/World/Foundation/PreCookReport.h"
#include "Engine/World/Foundation/TransientVisualElement.h"

#include <Common/assertion.h>
#include <Common/logging.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(TransformedInstanceActor, Actor);

PreCookReport ATransformedInstance::preCook(const CookingContext& ctx) const
{
	PreCookReport report = PhysicalActor::preCook(ctx);
	if(!m_source)
	{
		PH_LOG(TransformedInstanceActor, Warning,
			"ignoring this transformed instance: source actor is not specified");
		return report.markAsUncookable();
	}

	auto* localToWorld = ctx.getResources().makeTransform<StaticAffineTransform>(
		m_localToWorld.getForwardStaticAffine());
	auto* worldToLocal = ctx.getResources().makeTransform<StaticAffineTransform>(
		m_localToWorld.getInverseStaticAffine());
	report.setBaseTransforms(localToWorld, worldToLocal);

	return report;
}

TransientVisualElement ATransformedInstance::cook(
	const CookingContext& ctx, const PreCookReport& report) const
{
	const TransientVisualElement* sourceElement = ctx.getCached(m_source);
	if(!sourceElement)
	{
		throw ActorCookException(
			"transformed instance source dependency was not cooked and cached");
	}

	if(!sourceElement->surfaceEmitters.empty())
	{
		throw ActorCookException(
			"transformed instance does not support emitting source actors");
	}

	if(sourceElement->intersectables.size() > 1)
	{
		throw ActorCookException(
			"transformed instance source produced multiple intersectables; use "
			"is-instantiable-hint on a source actor that supports instancing");
	}

	if(sourceElement->intersectables.empty())
	{
		return {};
	}

	const auto* localToWorld = report.getBaseLocalToWorld();
	const auto* worldToLocal = report.getBaseWorldToLocal();
	PH_ASSERT(localToWorld);
	PH_ASSERT(worldToLocal);

	auto* transformed = ctx.getResources().copyIntersectable(
		IntersectableBuilder::referencing(sourceElement->intersectables.front())
			.transform(localToWorld, worldToLocal)
			.build());

	TransientVisualElement result;
	result.intersectables.push_back(transformed);
	return result;
}

void ATransformedInstance::setSource(const std::shared_ptr<Actor>& source)
{
	m_source = source;
}

}// end namespace ph
