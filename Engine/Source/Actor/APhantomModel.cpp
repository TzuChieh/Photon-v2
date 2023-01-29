#include "Actor/APhantomModel.h"
#include "Math/math.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "World/Foundation/CookedUnit.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Core/Intersectable/TransformedIntersectable.h"
#include "Actor/MotionSource/MotionSource.h"
#include "Core/Quantity/Time.h"
#include "Actor/ModelBuilder.h"
#include "World/Foundation/CookingContext.h"
#include "Core/Intersectable/Bvh/ClassicBvhIntersector.h"
#include "World/Foundation/CookOrder.h"

#include <algorithm>
#include <memory>
#include <vector>

namespace ph
{

CookedUnit APhantomModel::cook(CookingContext& ctx, const PreCookReport& report)
{
	CookedUnit cooked = AModel::cook(ctx, report);

	std::vector<const Intersectable*> intersectables;
	for(auto& intersectable : cooked.intersectables())
	{
		intersectables.push_back(intersectable.get());
		cooked.addBackend(std::move(intersectable));
	}
	cooked.intersectables().clear();

	auto bvh = std::make_unique<ClassicBvhIntersector>();
	bvh->rebuildWithIntersectables(std::move(intersectables));
	cooked.addIntersectable(std::move(bvh));

	ctx.addPhantom(m_phantomName, std::move(cooked));

	return CookedUnit();
}

CookOrder APhantomModel::getCookOrder() const
{
	return CookOrder(ECookPriority::HIGH);
}

}// end namespace ph
