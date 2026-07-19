#include "Engine/Actor/Geometry/Geometry.h"
#include "Engine/Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Engine/World/Foundation/CookedGeometry.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/Actor/Basic/exceptions.h"

#include <Common/logging.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(Geometry, Geometry);

void Geometry::storeCookedWithBakedTransform(
	const CookingContext& ctx,
	const StaticAffineTransform& transform,
	CookedGeometry& out_geometry) const
{
	auto triangulatedGeometry = genTriangulated();
	if(triangulatedGeometry == nullptr)
	{
		throw_formatted<CookException>(
			"geometry does not support baked transforms (id: {})", getId());
	}

	triangulatedGeometry->storeCookedWithBakedTransform(ctx, transform, out_geometry);
}

std::shared_ptr<Geometry> Geometry::genTriangulated() const
{
	return nullptr;
}

void Geometry::cook(const CookingContext& ctx, CookedGeometry& out_geometry) const
{
	const GeometryCookingConfig& config = ctx.getGeometryConfig();

	std::shared_ptr<Geometry> triangulatedGeometry;
	if(config.forceTriangulated)
	{
		triangulatedGeometry = genTriangulated();
		if(triangulatedGeometry == nullptr)
		{
			throw_formatted<CookException>(
				"failed to force triangulation on geometry (id: {})", getId());
		}
	}

	const Geometry& geometry = triangulatedGeometry ? *triangulatedGeometry : *this;
	if(!config.forceBakedTransform || config.bakedTransform.isIdentity())
	{
		geometry.storeCooked(ctx, out_geometry);
		return;
	}

	const auto& transform = StaticAffineTransform::makeForward(config.bakedTransform);
	geometry.storeCookedWithBakedTransform(ctx, transform, out_geometry);
}

}// end namespace ph
