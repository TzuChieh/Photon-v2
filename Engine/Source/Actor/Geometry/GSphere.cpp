#include "Actor/Geometry/GSphere.h"
#include "Core/Intersectable/PTriangle.h"
#include "Actor/Geometry/GTriangle.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/Geometry/TSphere.h"
#include "Actor/AModel.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Actor/Geometry/GTriangleMesh.h"
#include "Core/Intersectable/PLatLong01Sphere.h"
#include "World/Foundation/CookingContext.h"
#include "World/Foundation/CookedResourceCollection.h"
#include "World/Foundation/CookedGeometry.h"

#include <cmath>
#include <iostream>

namespace ph
{

class IndexedTriangle
{
public:
	IndexedTriangle(const std::size_t iA, const std::size_t iB, const std::size_t iC) :
		iA(iA), iB(iB), iC(iC)
	{}

	std::size_t iA;
	std::size_t iB;
	std::size_t iC;
};

void GSphere::cook(
	CookedGeometry& out_geometry,
	const CookingContext& ctx,
	const GeometryCookConfig& config) const
{
	if(config.preferTriangulated)
	{
		genTriangleMesh()->cook(out_geometry, ctx, config);
	}
	else
	{
		out_geometry.primitives.push_back(
			ctx.getCooked()->makeIntersectable<PLatLong01Sphere>(m_radius));
	}
}

void GSphere::genPrimitive(
	const PrimitiveBuildingMaterial& data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	if(!data.metadata)
	{
		std::cerr << "warning: at GSphere::discretize(), " 
		          << "no PrimitiveMetadata" << std::endl;
		return;
	}

	//genTriangleMesh()->genPrimitive(data, out_primitives);

	out_primitives.push_back(std::make_unique<PLatLong01Sphere>(m_radius));
}

std::shared_ptr<Geometry> GSphere::genTransformed(
	const math::StaticAffineTransform& transform) const
{
	return genTriangleMesh()->genTransformed(transform);
}

GSphere& GSphere::setRadius(const real radius)
{
	m_radius = radius;

	return *this;
}

std::size_t GSphere::addVertex(const math::Vector3R& vertex, std::vector<math::Vector3R>* const out_vertices) const
{
	// make vertex on the sphere
	math::Vector3R vertexOnSphere(vertex);
	vertexOnSphere.normalizeLocal().mulLocal(m_radius);

	out_vertices->push_back(vertexOnSphere);

	return out_vertices->size() - 1;
}

std::size_t GSphere::addMidpointVertex(const std::size_t iA, const std::size_t iB, 
                                       std::vector<math::Vector3R>* const out_vertices) const
{
	return addVertex((*out_vertices)[iA].add((*out_vertices)[iB]).mulLocal(0.5f), out_vertices);
}

std::shared_ptr<GTriangleMesh> GSphere::genTriangleMesh() const
{
	// Discretize the sphere into an icosphere

	// TODO: check data

	const uint32 nRefinements = 5;

	std::vector<math::Vector3R>  vertices;
	std::vector<IndexedTriangle> indexedTriangles;

	// 12 vertices of a icosahedron (located on the sphere)

	const real t = (1.0_r + sqrt(5.0_r)) / 2.0_r;

	// xy-plane
	addVertex(math::Vector3R(-1,  t,  0), &vertices);// 0
	addVertex(math::Vector3R( 1,  t,  0), &vertices);// 1
	addVertex(math::Vector3R(-1, -t,  0), &vertices);// 2
	addVertex(math::Vector3R( 1, -t,  0), &vertices);// 3

	// yz-plane
	addVertex(math::Vector3R( 0, -1,  t), &vertices);// 4
	addVertex(math::Vector3R( 0,  1,  t), &vertices);// 5
	addVertex(math::Vector3R( 0, -1, -t), &vertices);// 6
	addVertex(math::Vector3R( 0,  1, -t), &vertices);// 7

	// zx-plane
	addVertex(math::Vector3R( t,  0, -1), &vertices);// 8
	addVertex(math::Vector3R( t,  0,  1), &vertices);// 9
	addVertex(math::Vector3R(-t,  0, -1), &vertices);// 10
	addVertex(math::Vector3R(-t,  0,  1), &vertices);// 11

	// Generate 20 triangles from the icosahedron (all CCW)

	// 5 triangles around vertex 0
	indexedTriangles.push_back(IndexedTriangle(0, 11,  5));
	indexedTriangles.push_back(IndexedTriangle(0,  5,  1));
	indexedTriangles.push_back(IndexedTriangle(0,  1,  7));
	indexedTriangles.push_back(IndexedTriangle(0,  7, 10));
	indexedTriangles.push_back(IndexedTriangle(0, 10, 11));

	// Above's 5 adjacent triangles
	indexedTriangles.push_back(IndexedTriangle( 1,  5, 9));
	indexedTriangles.push_back(IndexedTriangle( 5, 11, 4));
	indexedTriangles.push_back(IndexedTriangle(11, 10, 2));
	indexedTriangles.push_back(IndexedTriangle(10,  7, 6));
	indexedTriangles.push_back(IndexedTriangle( 7,  1, 8));

	// 5 triangles around point 3
	indexedTriangles.push_back(IndexedTriangle(3, 9, 4));
	indexedTriangles.push_back(IndexedTriangle(3, 4, 2));
	indexedTriangles.push_back(IndexedTriangle(3, 2, 6));
	indexedTriangles.push_back(IndexedTriangle(3, 6, 8));
	indexedTriangles.push_back(IndexedTriangle(3, 8, 9));

	// Above's 5 adjacent triangles
	indexedTriangles.push_back(IndexedTriangle(4, 9,  5));
	indexedTriangles.push_back(IndexedTriangle(2, 4, 11));
	indexedTriangles.push_back(IndexedTriangle(6, 2, 10));
	indexedTriangles.push_back(IndexedTriangle(8, 6,  7));
	indexedTriangles.push_back(IndexedTriangle(9, 8,  1));

	// Refine triangles
	for(uint32 i = 0; i < nRefinements; i++)
	{
		std::vector<IndexedTriangle> refined;
		for(const IndexedTriangle& triangle : indexedTriangles)
		{
			// split a triangle into 4 triangles
			const std::size_t i1 = addMidpointVertex(triangle.iA, triangle.iB, &vertices);
			const std::size_t i2 = addMidpointVertex(triangle.iB, triangle.iC, &vertices);
			const std::size_t i3 = addMidpointVertex(triangle.iC, triangle.iA, &vertices);

			refined.push_back(IndexedTriangle(triangle.iA, i1, i3));
			refined.push_back(IndexedTriangle(triangle.iB, i2, i1));
			refined.push_back(IndexedTriangle(triangle.iC, i3, i2));
			refined.push_back(IndexedTriangle(i1, i2, i3));
		}

		indexedTriangles = refined;
	}

	math::TSphere referenceSphere(m_radius);

	// Construct actual triangles
	auto triangleMesh = TSdl<GTriangleMesh>::makeResource();
	for(const IndexedTriangle& iTriangle : indexedTriangles)
	{
		math::Vector3R vA(vertices[iTriangle.iA]);
		math::Vector3R vB(vertices[iTriangle.iB]);
		math::Vector3R vC(vertices[iTriangle.iC]);

		auto triangle = TSdl<GTriangle>::make().setVertices(vA, vB, vC);
		triangle.setNa(vA.normalize());
		triangle.setNb(vB.normalize());
		triangle.setNc(vC.normalize());

		const auto uvA = referenceSphere.surfaceToLatLong01(vA);
		triangle.setUVWa({uvA.x(), uvA.y(), 0.0_r});

		const auto uvB = referenceSphere.surfaceToLatLong01(vB);
		triangle.setUVWb({uvB.x(), uvB.y(), 0.0_r});

		const auto uvC = referenceSphere.surfaceToLatLong01(vC);
		triangle.setUVWc({uvC.x(), uvC.y(), 0.0_r});

		triangleMesh->addTriangle(triangle);
	}

	return triangleMesh;
}

}// end namespace ph
