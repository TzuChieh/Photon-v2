#include "Engine/Actor/AMaskedModel.h"
#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/World/Foundation/PreCookReport.h"
#include "Engine/World/Foundation/TransientVisualElement.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/Core/Intersection/BVH/TBinaryBvhIntersector.h"
#include "Engine/Core/Intersection/MaskedIntersectable.h"

#include <Common/logging.h>

namespace ph
{

PreCookReport AMaskedModel::preCook(const CookingContext& ctx) const
{
	PreCookReport report = Actor::preCook(ctx);
	if(!report.isCookable())
	{
		return report;
	}

	if(!m_base)
	{
		PH_LOG(ActorCooking, Warning,
			"ignoring this masked model: base model is not specified");
		report.markAsUncookable();
	}

	if(!m_mask)
	{
		PH_LOG(ActorCooking, Warning,
			"ignoring this masked model: mask is not specified");
		report.markAsUncookable();
	}

	return report;
}

TransientVisualElement AMaskedModel::cook(const CookingContext& ctx, const PreCookReport& report) const
{
	const TransientVisualElement* baseResult = ctx.getCached(m_base);
	if(!baseResult)
	{
		throw ActorCookException(
			"masked model base dependency was not cooked and cached");
	}
	TransientVisualElement result = *baseResult;

	// Cannot have primitive view as the intersectables will be further masked
	result.primitivesView.clear();

	std::shared_ptr<TTexture<real>> maskTexture = m_mask->genRealTexture(ctx);
	for(auto& isable : result.intersectables)
	{
		auto* maskedIsable = ctx.getResources().makeIntersectable<MaskedIntersectable>(
			isable, maskTexture);
		isable = maskedIsable;
	}

	if(isInstantiableHint() && result.intersectables.size() > 1)
	{
		auto* aggregate = ctx.getResources().makeIntersectable<TBinaryBvhIntersector<uint32>>();
		aggregate->update(result.intersectables);
		result.intersectables = {aggregate};
	}

	return result;
}

void AMaskedModel::setMask(const std::shared_ptr<Image>& mask)
{
	m_mask = mask;
}

}// end namespace ph
