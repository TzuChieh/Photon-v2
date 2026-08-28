#include <Engine/Core/Intersection/PLatLong01Sphere.h>
#include <Engine/Core/Intersection/TPIndexedKdTreeTriangleMesh.h>
#include <Engine/Core/Intersection/DataStructure/TIndexedPolygonBuffer.h>
#include <Engine/Core/Intersection/PrimitiveMetadata.h>
#include <Engine/Core/Ray.h>

#include <gtest/gtest.h>

#include <array>
#include <limits>
#include <memory>

using namespace ph;
using namespace ph::math;

TEST(PrimitiveIntersectionTest, RaySphereIntersection)
{
	std::unique_ptr<Intersectable> unitSphere = std::make_unique<PLatLong01Sphere>(1.0_r);

	Ray longXAxisRay(
		Vector3R(-100000.0_r, 0, 0), 
		Vector3R(1, 0, 0), 
		0, 
		std::numeric_limits<real>::max());
	EXPECT_TRUE(unitSphere->isOccluding(longXAxisRay));

	Ray shortXAxisRay(
		Vector3R(-100000.0_r, 0, 0),
		Vector3R(1, 0, 0), 
		0, 
		1);
	EXPECT_FALSE(unitSphere->isOccluding(shortXAxisRay));

	Ray insideUnitSphereRay(
		Vector3R(0, 0, 0), 
		Vector3R(1, 0, 0), 
		0, 
		0.1_r);
	EXPECT_FALSE(unitSphere->isOccluding(insideUnitSphereRay));

	Ray fromInsideToOutsideUnitSphereRay(
		Vector3R(0, 0, 0), 
		Vector3R(1, 0, 0), 
		0, 
		std::numeric_limits<real>::max());
	EXPECT_TRUE(unitSphere->isOccluding(fromInsideToOutsideUnitSphereRay));
}

TEST(PrimitiveIntersectionTest, RayTriangleMeshOcclusion)
{
	IndexedTriangleBuffer triangleBuffer;
	auto& vertexBuffer = triangleBuffer.getAttributeBuffer();
	vertexBuffer.declareAttribute(
		EPrimitiveAttribute::Position_0,
		EAttributeDomain::Vertex,
		EAttributeElement::Float32,
		3);
	auto vertexWriter = vertexBuffer.allocate(3);
	vertexWriter.setAttribute(EPrimitiveAttribute::Position_0, 0, {-1, -1, 0});
	vertexWriter.setAttribute(EPrimitiveAttribute::Position_0, 1, { 1, -1, 0});
	vertexWriter.setAttribute(EPrimitiveAttribute::Position_0, 2, { 0,  1, 0});

	auto& indexBuffer = triangleBuffer.getIndexBuffer();
	indexBuffer.declareUIntFormat<uint32>();
	indexBuffer.allocate(3);
	const uint32 indices[] = {0, 1, 2};
	indexBuffer.setUInts(indices, 3);

	const TPIndexedKdTreeTriangleMesh<uint32> mesh(&triangleBuffer);

	EXPECT_TRUE(mesh.isOccluding(Ray({0, 0, -1}, {0, 0, 1}, 0, 2)));
	EXPECT_FALSE(mesh.isOccluding(Ray({0, 0, -1}, {0, 0, 1}, 0, 0.5_r)));
	EXPECT_FALSE(mesh.isOccluding(Ray({2, 0, -1}, {0, 0, 1}, 0, 2)));
}

TEST(PrimitiveIntersectionTest, TriangleMeshRetrievesAttributes)
{
	IndexedTriangleBuffer triangleBuffer;
	auto& attributeBuffer = triangleBuffer.getAttributeBuffer();
	attributeBuffer.declareAttribute(
		EPrimitiveAttribute::Position_0,
		EAttributeDomain::Vertex,
		EAttributeElement::Float32,
		3);
	attributeBuffer.declareAttribute(
		EPrimitiveAttribute::Custom_0,
		EAttributeDomain::Face,
		EAttributeElement::Float32,
		1);
	auto attributeWriter = attributeBuffer.allocate(3, 1);
	attributeWriter.setAttribute(EPrimitiveAttribute::Position_0, 0, {-1, -1, 0});
	attributeWriter.setAttribute(EPrimitiveAttribute::Position_0, 1, { 1, -1, 0});
	attributeWriter.setAttribute(EPrimitiveAttribute::Position_0, 2, { 0,  1, 0});
	attributeWriter.setAttribute(EPrimitiveAttribute::Custom_0, 0, 0.75_r);

	auto& indexBuffer = triangleBuffer.getIndexBuffer();
	indexBuffer.declareUIntFormat<uint32>();
	indexBuffer.allocate(3);
	const uint32 indices[] = {0, 1, 2};
	indexBuffer.setUInts(indices, 3);

	const TPIndexedKdTreeTriangleMesh<uint32> mesh(&triangleBuffer);

	std::array<Vector3R, 1> faceValues;
	ASSERT_EQ(
		mesh.getAttribute(EPrimitiveAttribute::Custom_0, EAttributeDomain::Face, 0, faceValues),
		faceValues.size());
	EXPECT_EQ(faceValues[0], Vector3R(0.75_r, 0, 0));

	std::array<Vector3R, 3> vertexValues;
	ASSERT_EQ(
		mesh.getAttribute(EPrimitiveAttribute::Position_0, EAttributeDomain::Vertex, 0, vertexValues),
		vertexValues.size());
	EXPECT_EQ(vertexValues, (std::array{Vector3R(-1, -1, 0), Vector3R(1, -1, 0), Vector3R(0, 1, 0)}));
}
