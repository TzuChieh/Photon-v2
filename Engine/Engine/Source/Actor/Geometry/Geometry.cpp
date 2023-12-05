#include "Actor/Geometry/Geometry.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "World/Foundation/CookedGeometry.h"
#include "World/Foundation/CookingContext.h"
#include "Actor/Basic/exceptions.h"
#include "Common/logging.h"

#include <Common/assertion.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(Geometry, Geometry);

std::shared_ptr<Geometry> Geometry::genTransformed(
	const math::StaticAffineTransform& transform) const
{
	auto triangulatedGeometry = genTriangulated();
	if(triangulatedGeometry == nullptr)
	{
		return nullptr;
	}

	return triangulatedGeometry->genTransformed(transform);
}

std::shared_ptr<Geometry> Geometry::genTriangulated() const
{
	return nullptr;
}

CookedGeometry* Geometry::createCooked(const CookingContext& ctx) const
{
	CookedGeometry* cookedGeometry = nullptr;
	if(ctx.getConfig().forceTriangulated)
	{
		auto transientGeometry = genTriangulated();
		if(transientGeometry == nullptr)
		{
			throw_formatted<CookException>(
				"failed to force triangulation on geometry (id: {})", getId());
		}

		// Using the original geometry's ID since we want it to have triangulated result
		cookedGeometry = ctx.getResources()->makeGeometry(getId());

		transientGeometry->storeCooked(*cookedGeometry, ctx);
	}
	else
	{
		cookedGeometry = ctx.getResources()->makeGeometry(getId());
		PH_ASSERT(cookedGeometry);

		storeCooked(*cookedGeometry, ctx);
	}

	return cookedGeometry;
}

}// end namespace ph
