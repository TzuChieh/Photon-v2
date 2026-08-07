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

#include <array>
#include <cstddef>
#include <utility>

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

	if(m_transforms.empty())
	{
		return report.markAsUncookable();
	}

	return report;
}

TransientVisualElement ATransformedInstance::cook(
	const CookingContext& ctx, const PreCookReport& /* report */) const
{
	PH_ASSERT(!m_transforms.empty());

	const TransientVisualElement* sourceElement = ctx.getCached(m_source);
	if(!sourceElement)
	{
		throw ActorCookException(
			"transformed instance source was not cooked");
	}

	if(sourceElement->intersectables.size() != 1 || !sourceElement->surfaceEmitters.empty())
	{
		throw ActorCookException(
			"transformed instance source must produce exactly one non-emitting intersectable");
	}

	const Intersectable* const source = sourceElement->intersectables.front();
	const math::TDecomposedTransform<real>& baseTransform = m_localToWorld.getDecomposed();
	
	auto& cooked = ctx.getResources();
	TransientVisualElement result;
	result.intersectables.reserve(m_transforms.size());
	for(std::size_t instanceIdx = 0; instanceIdx < m_transforms.size(); ++instanceIdx)
	{
		const TransformInfo& instanceTransform = m_transforms[instanceIdx];
		const std::array transformChain = {instanceTransform.getDecomposed(), baseTransform};

		auto* localToWorld = cooked.makeTransform<StaticAffineTransform>(
			StaticAffineTransform::makeParentedForward<real>(transformChain));
		auto* worldToLocal = cooked.makeTransform<StaticAffineTransform>(
			StaticAffineTransform::makeParentedInverse<real>(transformChain));

		auto* transformed = cooked.copyIntersectable(
			IntersectableBuilder::referencing(source)
				.transform(localToWorld, worldToLocal)
				.build());
		result.intersectables.push_back(transformed);
	}

	return result;
}

void ATransformedInstance::setSource(const std::shared_ptr<Actor>& source)
{
	m_source = source;
}

void ATransformedInstance::setTransforms(std::vector<TransformInfo> transforms)
{
	m_transforms = std::move(transforms);
}

}// end namespace ph
