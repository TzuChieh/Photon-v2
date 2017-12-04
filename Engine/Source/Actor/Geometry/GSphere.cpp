#include "Actor/Geometry/GSphere.h"
#include "Core/Intersectable/PTriangle.h"
#include "Actor/Geometry/GTriangle.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/StGenerator/StGenerator.h"
#include "Actor/AModel.h"
#include "FileIO/InputPacket.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Actor/Geometry/GTriangleMesh.h"

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

GSphere::GSphere(const real radius) :
	Geometry(), 
	m_radius(radius)
{

}

GSphere::GSphere(const GSphere& other) :
	Geometry(), 
	m_radius(other.m_radius)
{

}

GSphere::~GSphere() = default;

// discretize the sphere into an icosphere
void GSphere::genPrimitive(const PrimitiveBuildingMaterial& data,
                           std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	if(!data.metadata)
	{
		std::cerr << "warning: at GSphere::discretize(), " 
		          << "no PrimitiveMetadata" << std::endl;
		return;
	}

	/*if(parentModel.getScale().x != parentModel.getScale().y || parentModel.getScale().y != parentModel.getScale().z)
	{
		std::cerr << "warning: nonuniform scale on GSphere detected" << std::endl;
		return;
	}*/

	genTriangleMesh()->genPrimitive(data, out_primitives);
}

std::shared_ptr<Geometry> GSphere::genTransformApplied(const StaticTransform& transform) const
{
	return genTriangleMesh()->genTransformApplied(transform);
}

std::size_t GSphere::addVertex(const Vector3R& vertex, std::vector<Vector3R>* const out_vertices) const
{
	// make vertex on the sphere
	Vector3R vertexOnSphere(vertex);
	vertexOnSphere.normalizeLocal().mulLocal(m_radius);

	out_vertices->push_back(vertexOnSphere);

	return out_vertices->size() - 1;
}

std::size_t GSphere::addMidpointVertex(const std::size_t iA, const std::size_t iB, 
                                       std::vector<Vector3R>* const out_vertices) const
{
	return addVertex((*out_vertices)[iA].add((*out_vertices)[iB]).mulLocal(0.5f), out_vertices);
}

std::shared_ptr<GTriangleMesh> GSphere::genTriangleMesh() const
{
	// TODO: check data

	const uint32 nRefinements = 5;

	std::vector<Vector3R>        vertices;
	std::vector<IndexedTriangle> indexedTriangles;

	// 12 vertices of a icosahedron (located on the sphere)

	const real t = (1.0_r + sqrt(5.0_r)) / 2.0_r;

	// xy-plane
	addVertex(Vector3R(-1,  t,  0), &vertices);// 0
	addVertex(Vector3R( 1,  t,  0), &vertices);// 1
	addVertex(Vector3R(-1, -t,  0), &vertices);// 2
	addVertex(Vector3R( 1, -t,  0), &vertices);// 3

	// yz-plane
	addVertex(Vector3R( 0, -1,  t), &vertices);// 4
	addVertex(Vector3R( 0,  1,  t), &vertices);// 5
	addVertex(Vector3R( 0, -1, -t), &vertices);// 6
	addVertex(Vector3R( 0,  1, -t), &vertices);// 7

	// zx-plane
	addVertex(Vector3R( t,  0, -1), &vertices);// 8
	addVertex(Vector3R( t,  0,  1), &vertices);// 9
	addVertex(Vector3R(-t,  0, -1), &vertices);// 10
	addVertex(Vector3R(-t,  0,  1), &vertices);// 11

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

	auto& triangleMesh = std::make_shared<GTriangleMesh>();

	// construct actual triangles
	for(const IndexedTriangle& iTriangle : indexedTriangles)
	{
		Vector3R vA(vertices[iTriangle.iA]);
		Vector3R vB(vertices[iTriangle.iB]);
		Vector3R vC(vertices[iTriangle.iC]);

		GTriangle triangle(vA, vB, vC);
		triangle.setNa(vA.normalize());
		triangle.setNb(vB.normalize());
		triangle.setNc(vC.normalize());

		Vector3R mappedUVW;

		m_stGenerator->map(vA, triangle.getUVWa(), &mappedUVW);
		triangle.setUVWa(mappedUVW);

		m_stGenerator->map(vB, triangle.getUVWb(), &mappedUVW);
		triangle.setUVWb(mappedUVW);

		m_stGenerator->map(vC, triangle.getUVWc(), &mappedUVW);
		triangle.setUVWc(mappedUVW);

		triangleMesh->addTriangle(triangle);
	}

	return triangleMesh;
}

GSphere& GSphere::operator = (const GSphere& rhs)
{
	m_radius = rhs.m_radius;

	return *this;
}

// command interface

SdlTypeInfo GSphere::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_GEOMETRY, "sphere");
}

void GSphere::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<GSphere>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<GSphere> GSphere::ciLoad(const InputPacket& packet)
{
	const real radius = packet.getReal("radius", 1.0_r);

	return std::make_unique<GSphere>(radius);
}

}// end namespace ph