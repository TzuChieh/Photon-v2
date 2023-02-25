#include "Actor/Geometry/Geometry.h"
#include "Common/assertion.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "World/Foundation/CookedGeometry.h"
#include "World/Foundation/CookingContext.h"
#include "Actor/Basic/exceptions.h"
#include "Common/logging.h"

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

CookedGeometry* Geometry::genCooked(
	const CookingContext& ctx,
	const GeometryCookConfig& config) const
{
	CookedGeometry* cookedGeometry = nullptr;
	if(config.forceTriangulated)
	{
		auto transientGeometry = genTriangulated();
		if(transientGeometry == nullptr)
		{
			throw_formatted<CookException>(
				"failed to force triangulation on geometry (id: {})", getId());
		}

		// Using the original geometry's ID since we want it to have triangulated result
		cookedGeometry = ctx.getCooked()->makeGeometry(getId());

		transientGeometry->cook(*cookedGeometry, ctx, config);
	}
	else
	{
		cookedGeometry = ctx.getCooked()->makeGeometry(getId());
		PH_ASSERT(cookedGeometry);

		cook(*cookedGeometry, ctx, config);
	}

	return cookedGeometry;
}

}// end namespace ph
