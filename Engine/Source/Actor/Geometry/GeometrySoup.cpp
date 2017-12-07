#include "Actor/Geometry/GeometrySoup.h"

#include <iostream>

namespace ph
{

GeometrySoup::GeometrySoup() : 
	Geometry(),
	m_geometries()
{

}

GeometrySoup::~GeometrySoup() = default;

void GeometrySoup::genPrimitive(const PrimitiveBuildingMaterial& data,
                                std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	for(const auto& geometry : m_geometries)
	{
		geometry->genPrimitive(data, out_primitives);
	}
}

std::shared_ptr<Geometry> GeometrySoup::genTransformApplied(const StaticTransform& transform) const
{
	auto tGeometrySoup = std::make_shared<GeometrySoup>();
	for(const auto& geometry : m_geometries)
	{
		const auto& tGeometry = geometry->genTransformApplied(transform);
		if(tGeometry == nullptr)
		{
			std::cerr << "warning: at GeometrySoup::genTransformApplied(), "
			          << "a geometry cannot apply specified transform" << std::endl;
			continue;
		}

		tGeometrySoup->m_geometries.push_back(tGeometry);
	}

	return tGeometrySoup;
}

void GeometrySoup::addGeometry(const std::shared_ptr<Geometry>& geometry)
{
	m_geometries.push_back(geometry);
}

}// end namespace ph