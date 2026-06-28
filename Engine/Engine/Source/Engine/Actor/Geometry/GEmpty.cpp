#include "Engine/Actor/Geometry/GEmpty.h"
#include "Engine/Core/Intersection/PEmpty.h"
#include "Engine/Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

namespace ph
{

void GEmpty::storeCooked(
	const CookingContext& ctx,
	CookedGeometry& out_geometry) const
{
	out_geometry.primitives.push_back(
		ctx.getResources().makeIntersectable<PEmpty>());
}

}// end namespace ph
