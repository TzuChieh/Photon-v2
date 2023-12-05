#include "Actor/ATransformedInstance.h"
#include "Math/math.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "World/Foundation/TransientVisualElement.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Core/Intersectable/TransformedIntersectable.h"
#include "Actor/MotionSource/MotionSource.h"
#include "Core/Quantity/Time.h"
#include "Actor/ModelBuilder.h"
#include "World/Foundation/CookingContext.h"

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

	auto transformedTarget = std::make_unique<TransformedIntersectable>(
		phantom->intersectables().front().get(),
		baseLW.get(),
		baseWL.get());

	cooked.addIntersectable(std::move(transformedTarget));
	cooked.addTransform(std::move(baseLW));
	cooked.addTransform(std::move(baseWL));*/
	PH_ASSERT_UNREACHABLE_SECTION();

	return cooked;
}

}// end namespace ph
