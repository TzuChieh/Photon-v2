#include "Engine/Actor/Geometry/Geometry.h"
#include "Engine/Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Engine/World/Foundation/CookedGeometry.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/Actor/Basic/exceptions.h"

#include <Common/logging.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(Geometry, Geometry);

std::shared_ptr<Geometry> Geometry::genTransformed(
	const StaticAffineTransform& transform) const
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

void Geometry::cook(const CookingContext& ctx, CookedGeometry& out_geometry) const
{
	if(ctx.getConfig().forceTriangulated)
	{
		auto transientGeometry = genTriangulated();
		if(transientGeometry == nullptr)
		{
			throw_formatted<CookException>(
				"failed to force triangulation on geometry (id: {})", getId());
		}

		transientGeometry->storeCooked(ctx, out_geometry);
	}
	else
	{
		storeCooked(ctx, out_geometry);
	}
}

}// end namespace ph
