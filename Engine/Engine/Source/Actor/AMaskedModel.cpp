#include "Actor/AMaskedModel.h"
#include "World/Foundation/PreCookReport.h"
#include "World/Foundation/TransientVisualElement.h"
#include "World/Foundation/CookingContext.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "World/Foundation/TransientResourceCache.h"
#include "Core/Intersection/MaskedIntersectable.h"
#include "Actor/Image/SwizzledImage.h"

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
	TransientVisualElement result;

	const TransientVisualElement* baseResult = ctx.getCached(m_base);
	if(baseResult)
	{
		result = *baseResult;
	}
	else
	{
		// Cook new visual element if not cached
		result = m_base->stagelessCook(ctx);
	}

	// Cannot have primitive view as the intersectables will be further masked
	result.primitivesView.clear();

	std::shared_ptr<TTexture<real>> maskTexture;
	{
		auto mask = TSdl<SwizzledImage>::makeResource();
		mask->setInput(m_mask);
		mask->setSwizzleSubscripts("x");

		maskTexture = mask->genRealTexture(ctx);
	}

	for(auto& isable : result.intersectables)
	{
		auto* maskedIsable = ctx.getResources()->makeIntersectable<MaskedIntersectable>(
			isable, maskTexture);
		isable = maskedIsable;
	}

	return result;
}

void AMaskedModel::setMask(const std::shared_ptr<Image>& mask)
{
	m_mask = mask;
}

}// end namespace ph
