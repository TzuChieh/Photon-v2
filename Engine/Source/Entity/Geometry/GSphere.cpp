#include "Entity/Geometry/GSphere.h"
#include "Entity/Primitive/PTriangle.h"
#include "Math/Vector3f.h"
#include "Entity/Entity.h"
#include "Entity/TextureMapper/TextureMapper.h"

#include <cmath>
#include <iostream>

namespace ph
{

class IndexedTriangle
{
public:
	IndexedTriangle(const std::size_t iA, const std::size_t iB, const std::size_t iC) :
		iA(iA), iB(iB), iC(iC)
	{

	}

	std::size_t iA;
	std::size_t iB;
	std::size_t iC;
};

GSphere::GSphere(const float32 radius) :
	m_radius(radius)
{

}

GSphere::GSphere(const GSphere& other) :
	m_radius(other.m_radius)
{

}

GSphere::~GSphere() = default;

// discretize the sphere into an icosphere
void GSphere::discretize(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const Entity* const parentEntity) const
{
	if(!(parentEntity->getScale().x == parentEntity->getScale().y && parentEntity->getScale().y == parentEntity->getScale().z))
	{
		std::cerr << "warning: nonuniform scale on GSphere detected" << std::endl;
	}

	const uint32 nRefinements = 5;

	std::vector<Vector3f>        vertices;
	std::vector<IndexedTriangle> indexedTriangles;

	// 12 vertices of a icosahedron (located on the sphere)

	const float32 t = (1.0f + sqrt(5.0f)) / 2.0f;

	// xy-plane
	addVertex(Vector3f(-1,  t,  0), &vertices);// 0
	addVertex(Vector3f( 1,  t,  0), &vertices);// 1
	addVertex(Vector3f(-1, -t,  0), &vertices);// 2
	addVertex(Vector3f( 1, -t,  0), &vertices);// 3

	// yz-plane
	addVertex(Vector3f( 0, -1,  t), &vertices);// 4
	addVertex(Vector3f( 0,  1,  t), &vertices);// 5
	addVertex(Vector3f( 0, -1, -t), &vertices);// 6
	addVertex(Vector3f( 0,  1, -t), &vertices);// 7

	// zx-plane
	addVertex(Vector3f( t,  0, -1), &vertices);// 8
	addVertex(Vector3f( t,  0,  1), &vertices);// 9
	addVertex(Vector3f(-t,  0, -1), &vertices);// 10
	addVertex(Vector3f(-t,  0,  1), &vertices);// 11

	// generate 20 triangles from the icosahedron (all CCW)

	// 5 triangles around vertex 0
	indexedTriangles.push_back(IndexedTriangle(0, 11,  5));
	indexedTriangles.push_back(IndexedTriangle(0,  5,  1));
	indexedTriangles.push_back(IndexedTriangle(0,  1,  7));
	indexedTriangles.push_back(IndexedTriangle(0,  7, 10));
	indexedTriangles.push_back(IndexedTriangle(0, 10, 11));

	// above's 5 adjacent triangles
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

	// above's 5 adjacent triangles
	indexedTriangles.push_back(IndexedTriangle(4, 9,  5));
	indexedTriangles.push_back(IndexedTriangle(2, 4, 11));
	indexedTriangles.push_back(IndexedTriangle(6, 2, 10));
	indexedTriangles.push_back(IndexedTriangle(8, 6,  7));
	indexedTriangles.push_back(IndexedTriangle(9, 8,  1));

	// refine triangles
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

	const auto* const textureMapper = parentEntity->getTextureMapper();

	// construct actual triangles
	for(const IndexedTriangle& iTriangle : indexedTriangles)
	{
		Vector3f vA(vertices[iTriangle.iA]);
		Vector3f vB(vertices[iTriangle.iB]);
		Vector3f vC(vertices[iTriangle.iC]);

		PTriangle triangle(parentEntity, vA, vB, vC);
		triangle.setNa(vA.normalize());
		triangle.setNb(vB.normalize());
		triangle.setNc(vC.normalize());

		Vector3f mappedUVW;

		textureMapper->map(vA, triangle.getUVWa(), &mappedUVW);
		triangle.setUVWa(mappedUVW);

		textureMapper->map(vB, triangle.getUVWb(), &mappedUVW);
		triangle.setUVWb(mappedUVW);

		textureMapper->map(vC, triangle.getUVWc(), &mappedUVW);
		triangle.setUVWc(mappedUVW);

		out_primitives->push_back(std::make_unique<PTriangle>(triangle));
	}
}

std::size_t GSphere::addVertex(const Vector3f& vertex, std::vector<Vector3f>* const out_vertices) const
{
	// make vertex on the sphere
	Vector3f vertexOnSphere(vertex);
	vertexOnSphere.normalizeLocal().mulLocal(m_radius);

	out_vertices->push_back(vertexOnSphere);

	return out_vertices->size() - 1;
}

std::size_t GSphere::addMidpointVertex(const std::size_t iA, const std::size_t iB, std::vector<Vector3f>* const out_vertices) const
{
	return addVertex((*out_vertices)[iA].add((*out_vertices)[iB]).mulLocal(0.5f), out_vertices);
}

GSphere& GSphere::operator = (const GSphere& rhs)
{
	m_radius = rhs.m_radius;

	return *this;
}

}// end namespace ph