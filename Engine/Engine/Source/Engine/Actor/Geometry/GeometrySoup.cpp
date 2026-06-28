#include "Engine/Actor/Geometry/GeometrySoup.h"
#include "Engine/Math/TDecomposedTransform.h"

#include <iostream>

namespace ph
{

GeometrySoup::GeometrySoup() : 
	Geometry(),
	m_geometries()
{}

void GeometrySoup::storeCooked(
	const CookingContext& ctx,
	CookedGeometry& out_geometry) const
{
	for(const auto& geometry : m_geometries)
	{
		geometry->storeCooked(ctx, out_geometry);
	}
}

std::shared_ptr<Geometry> GeometrySoup::genTransformed(
	const StaticAffineTransform& transform) const
{
	auto tGeometrySoup = TSdl<GeometrySoup>::makeResource();
	for(const auto& geometry : m_geometries)
	{
		const auto& tGeometry = geometry->genTransformed(transform);
		if(!tGeometry)
		{
			std::cerr << "warning: at GeometrySoup::genTransformApplied(), "
			          << "a geometry cannot apply specified transform" << std::endl;
			continue;
		}

		tGeometrySoup->m_geometries.push_back(tGeometry);
	}

	return tGeometrySoup;
}

void GeometrySoup::add(const std::shared_ptr<Geometry>& geometry)
{
	m_geometries.push_back(geometry);
}

bool GeometrySoup::addTransformed(
	const std::shared_ptr<Geometry>& geometry,
	const StaticAffineTransform& transform)
{
	const auto& transformed = geometry->genTransformed(transform);
	if(!transformed)
	{
		return false;
	}

	add(transformed);
	return true;
}

}// end namespace ph
