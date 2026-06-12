#include "Engine/Actor/APhantomModel.h"
#include "Engine/Math/math.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Actor/Geometry/Geometry.h"
#include "Engine/Actor/Material/Material.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/World/Foundation/TransientVisualElement.h"
#include "Engine/Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Engine/Actor/MotionSource/MotionSource.h"
#include "Engine/Core/Quantity/Time.h"
#include "Engine/Actor/ModelBuilder.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/Core/Intersection/BVH/TBinaryBvhIntersector.h"
#include "Engine/World/Foundation/CookOrder.h"

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
