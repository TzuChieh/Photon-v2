#include "Actor/APhantomModel.h"
#include "Math/math.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "World/Foundation/TransientVisualElement.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Core/Intersection/TransformedIntersectable.h"
#include "Actor/MotionSource/MotionSource.h"
#include "Core/Quantity/Time.h"
#include "Actor/ModelBuilder.h"
#include "World/Foundation/CookingContext.h"
#include "Core/Intersection/Bvh/ClassicBvhIntersector.h"
#include "World/Foundation/CookOrder.h"

#include <algorithm>
#include <memory>
#include <vector>

namespace ph
{

TransientVisualElement APhantomModel::cook(const CookingContext& ctx, const PreCookReport& report) const
{
	TransientVisualElement cooked = AModel::cook(ctx, report);

	/*std::vector<const Intersectable*> intersectables;
	for(auto& intersectable : cooked.intersectables())
	{
		intersectables.push_back(intersectable.get());
		cooked.addBackend(std::move(intersectable));
	}
	cooked.intersectables().clear();

	auto bvh = std::make_unique<ClassicBvhIntersector>();
	bvh->rebuildWithIntersectables(std::move(intersectables));
	cooked.addIntersectable(std::move(bvh));

	ctx.addPhantom(m_phantomName, std::move(cooked));*/
	PH_ASSERT_UNREACHABLE_SECTION();

	return TransientVisualElement();
}

CookOrder APhantomModel::getCookOrder() const
{
	return CookOrder(ECookPriority::High);
}

}// end namespace ph
