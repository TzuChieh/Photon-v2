#include "Engine/Actor/Geometry/GeometrySoup.h"

namespace ph
{

void GeometrySoup::storeCooked(
	const CookingContext& ctx,
	CookedGeometry& out_geometry) const
{
	for(const auto& geometry : m_geometries)
	{
		geometry->storeCooked(ctx, out_geometry);
	}
}

void GeometrySoup::storeCookedWithBakedTransform(
	const CookingContext& ctx,
	const StaticAffineTransform& transform,
	CookedGeometry& out_geometry) const
{
	for(const auto& geometry : m_geometries)
	{
		geometry->storeCookedWithBakedTransform(ctx, transform, out_geometry);
	}
}

void GeometrySoup::add(const std::shared_ptr<Geometry>& geometry)
{
	m_geometries.push_back(geometry);
}

}// end namespace ph
