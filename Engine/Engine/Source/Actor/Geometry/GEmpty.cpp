#include "Actor/Geometry/GEmpty.h"
#include "Core/Intersectable/PEmpty.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "World/Foundation/CookingContext.h"
#include "World/Foundation/CookedResourceCollection.h"

namespace ph
{

void GEmpty::storeCooked(
	CookedGeometry& out_geometry,
	const CookingContext& ctx) const
{
	out_geometry.primitives.push_back(
		ctx.getResources()->makeIntersectable<PEmpty>());
}

void GEmpty::genPrimitive(
	const PrimitiveBuildingMaterial&         data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	out_primitives.push_back(std::make_unique<PEmpty>());
}

}// end namespace ph
