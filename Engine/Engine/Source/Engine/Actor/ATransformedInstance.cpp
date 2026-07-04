#include "Engine/Actor/ATransformedInstance.h"
#include "Engine/Math/math.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Actor/Geometry/Geometry.h"
#include "Engine/Actor/Material/Material.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/World/Foundation/TransientVisualElement.h"
#include "Engine/Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Engine/Actor/MotionSource/MotionSource.h"
#include "Engine/Actor/ModelBuilder.h"
#include "Engine/World/Foundation/CookingContext.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <algorithm>
#include <memory>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(TransformedInstanceActor, Actor);

TransientVisualElement ATransformedInstance::cook(const CookingContext& ctx, const PreCookReport& report) const
{
	TransientVisualElement cooked;

	/*const CookedUnit* phantom = ctx.getPhantom(m_phantomName);
	if(!phantom)
	{
		PH_LOG_WARNING(TransformedInstanceActor, 
			"phantom <{}> not found", m_phantomName);

		return cooked;
	}

	if(phantom->intersectables().size() != 1)
	{
		PH_LOG_WARNING(TransformedInstanceActor, 
			"phantom <{}> contains unsupported data", m_phantomName);

		return cooked;
	}

	auto baseLW = std::make_unique<math::StaticAffineTransform>(math::StaticAffineTransform::makeForward(m_localToWorld));
	auto baseWL = std::make_unique<math::StaticAffineTransform>(math::StaticAffineTransform::makeInverse(m_localToWorld));

	auto transformedTarget = std::make_unique<
		TTransformedIntersectable<TReferencedIntersectableGetter<Intersectable>>>(
			TReferencedIntersectableGetter<Intersectable>(phantom->intersectables().front().get()),
			baseLW.get(),
			baseWL.get());

	cooked.addIntersectable(std::move(transformedTarget));
	cooked.addTransform(std::move(baseLW));
	cooked.addTransform(std::move(baseWL));*/
	PH_ASSERT_UNREACHABLE_SECTION();

	return cooked;
}

}// end namespace ph
