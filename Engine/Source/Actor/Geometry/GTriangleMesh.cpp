#include "Actor/Geometry/GTriangleMesh.h"
#include "Actor/Geometry/GTriangle.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/AModel.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Actor/Geometry/GeometrySoup.h"

#include <iostream>

namespace ph
{

GTriangleMesh::GTriangleMesh() : 
	Geometry(), 
	m_gTriangles()
{}

GTriangleMesh::GTriangleMesh(
	const std::vector<math::Vector3R>& positions,
	const std::vector<math::Vector3R>& texCoords,
	const std::vector<math::Vector3R>& normals) :
	GTriangleMesh()
{
	if(!(positions.size() == texCoords.size() && texCoords.size() == normals.size()) ||
	    (positions.empty() || texCoords.empty() || normals.empty()) ||
	    (positions.size() % 3 != 0 || texCoords.size() % 3 != 0 || normals.size() % 3 != 0))
	{
		std::cerr << "warning: at GTriangleMesh::GTriangleMesh(), " 
		          << "bad input detected" << std::endl;
		return;
	}

	for(std::size_t i = 0; i < positions.size(); i += 3)
	{
		GTriangle triangle(positions[i + 0], positions[i + 1], positions[i + 2]);
		if(triangle.isDegenerate())
		{
			continue;
		}

		triangle.setUVWa(texCoords[i + 0]);
		triangle.setUVWb(texCoords[i + 1]);
		triangle.setUVWc(texCoords[i + 2]);
		triangle.setNa(normals[i + 0].lengthSquared() > 0 ? normals[i + 0].normalize() : math::Vector3R(0, 1, 0));
		triangle.setNb(normals[i + 1].lengthSquared() > 0 ? normals[i + 1].normalize() : math::Vector3R(0, 1, 0));
		triangle.setNc(normals[i + 2].lengthSquared() > 0 ? normals[i + 2].normalize() : math::Vector3R(0, 1, 0));
		addTriangle(triangle);
	}
}

void GTriangleMesh::genPrimitive(
	const PrimitiveBuildingMaterial& data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	for(const auto& gTriangle : m_gTriangles)
	{
		gTriangle.genPrimitive(data, out_primitives);
	}
}

void GTriangleMesh::addTriangle(const GTriangle& gTriangle)
{
	m_gTriangles.push_back(gTriangle);
}

// TODO: can actually gen a transformed GTriangleMesh
std::shared_ptr<Geometry> GTriangleMesh::genTransformed(
	const math::StaticAffineTransform& transform) const
{
	auto geometrySoup = std::make_shared<GeometrySoup>();
	for(const auto& gTriangle : m_gTriangles)
	{
		geometrySoup->add(std::make_shared<GTriangle>(gTriangle));
	}

	return geometrySoup->genTransformed(transform);
}

}// end namespace ph