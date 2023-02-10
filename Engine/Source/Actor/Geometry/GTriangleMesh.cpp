#include "Actor/Geometry/GTriangleMesh.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Actor/Geometry/GeometrySoup.h"
#include "Actor/Basic/exceptions.h"

#include <utility>

namespace ph
{

GTriangleMesh::GTriangleMesh() : 
	GTriangleMesh({}, {}, {})
{}

GTriangleMesh::GTriangleMesh(
	std::vector<math::Vector3R> positions,
	std::vector<math::Vector3R> texCoords,
	std::vector<math::Vector3R> normals) :

	Geometry(),

	m_positions(std::move(positions)),
	m_texCoords(std::move(texCoords)),
	m_normals  (std::move(normals))
{}

void GTriangleMesh::cook(
	CookedGeometry& out_geometry,
	const CookingContext& ctx,
	const GeometryCookConfig& config) const
{
	const auto gTriangles = genTriangles();
	for(const auto& gTriangle : gTriangles)
	{
		gTriangle.cook(out_geometry, ctx, config);
	}
}

void GTriangleMesh::genPrimitive(
	const PrimitiveBuildingMaterial& data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	const auto gTriangles = genTriangles();
	for(const auto& gTriangle : gTriangles)
	{
		gTriangle.genPrimitive(data, out_primitives);
	}
}

std::vector<GTriangle> GTriangleMesh::genTriangles() const
{
	if(m_positions.empty() || m_positions.size() % 3 != 0)
	{
		throw CookException(
			"Triangle mesh with bad/zero position buffer size (" + std::to_string(m_positions.size()) + 
			" positions given). ");
	}

	if(m_texCoords.size() != m_positions.size())
	{
		throw CookException(
			"Triangle mesh with mismatched texture-coordinate buffer size (" + 
			std::to_string(m_texCoords.size()) + " texture coordinates given). ");
	}

	if(m_normals.size() != m_positions.size())
	{
		throw CookException(
			"Triangle mesh with mismatched normal buffer size (" +
			std::to_string(m_normals.size()) + " normal vectors given). ");
	}

	std::vector<GTriangle> gTriangles(m_positions.size() / 3);
	for(std::size_t i = 0; i < m_positions.size(); i += 3)
	{
		PH_ASSERT_LT(i + 2, m_positions.size());
		auto gTriangle = TSdl<GTriangle>::make().setVertices(
			m_positions[i + 0], m_positions[i + 1], m_positions[i + 2]);
		if(gTriangle.isDegenerate())
		{
			continue;
		}

		PH_ASSERT_LT(i + 2, m_texCoords.size());
		gTriangle.setUVWa(m_texCoords[i + 0]);
		gTriangle.setUVWb(m_texCoords[i + 1]);
		gTriangle.setUVWc(m_texCoords[i + 2]);

		PH_ASSERT_LT(i + 2, m_normals.size());
		gTriangle.setNa(m_normals[i + 0]);
		gTriangle.setNb(m_normals[i + 1]);
		gTriangle.setNc(m_normals[i + 2]);
		
		PH_ASSERT_LT(i / 3, gTriangles.size());
		gTriangles[i / 3] = gTriangle;
	}

	return gTriangles;
}

// TODO: can actually gen a transformed GTriangleMesh
std::shared_ptr<Geometry> GTriangleMesh::genTransformed(
	const math::StaticAffineTransform& transform) const
{
	const auto gTriangles = genTriangles();

	auto geometrySoup = std::make_shared<GeometrySoup>();
	for(const auto& gTriangle : gTriangles)
	{
		geometrySoup->add(std::make_shared<GTriangle>(gTriangle));
	}

	return geometrySoup->genTransformed(transform);
}

void GTriangleMesh::addTriangle(const GTriangle& gTriangle)
{
	m_positions.push_back(gTriangle.getVa());
	m_positions.push_back(gTriangle.getVb());
	m_positions.push_back(gTriangle.getVc());
	m_texCoords.push_back(gTriangle.getUVWa());
	m_texCoords.push_back(gTriangle.getUVWb());
	m_texCoords.push_back(gTriangle.getUVWc());
	m_normals.push_back(gTriangle.getNa());
	m_normals.push_back(gTriangle.getNb());
	m_normals.push_back(gTriangle.getNc());
}

}// end namespace ph