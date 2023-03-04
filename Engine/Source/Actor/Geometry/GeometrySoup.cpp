#include "Actor/Geometry/GeometrySoup.h"
#include "Math/Transform/TDecomposedTransform.h"

#include <iostream>

namespace ph
{

GeometrySoup::GeometrySoup() : 
	Geometry(),
	m_geometries()
{}

void GeometrySoup::storeCooked(
	CookedGeometry& out_geometry,
	const CookingContext& ctx,
	const GeometryCookConfig& config) const
{
	for(const auto& geometry : m_geometries)
	{
		geometry->storeCooked(out_geometry, ctx, config);
	}
}

void GeometrySoup::genPrimitive(
	const PrimitiveBuildingMaterial& data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	for(const auto& geometry : m_geometries)
	{
		geometry->genPrimitive(data, out_primitives);
	}
}

std::shared_ptr<Geometry> GeometrySoup::genTransformed(
	const math::StaticAffineTransform& transform) const
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
	const math::StaticAffineTransform& transform)
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
