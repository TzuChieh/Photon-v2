#include <Engine/Core/Intersection/PLatLong01Sphere.h>
#include <Engine/Core/Intersection/TPIndexedKdTreeTriangleMesh.h>
#include <Engine/Core/Intersection/DataStructure/TIndexedPolygonBuffer.h>
#include <Engine/Core/Ray.h>
#include <Engine/Core/Intersection/PrimitiveMetadata.h>

#include <gtest/gtest.h>

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
	auto& vertexBuffer = triangleBuffer.getVertexBuffer();
	vertexBuffer.declareAttribute(EVertexAttribute::Position_0, EVertexElement::Float32, 3);
	vertexBuffer.allocate(3);
	vertexBuffer.setAttribute(EVertexAttribute::Position_0, 0, {-1, -1, 0});
	vertexBuffer.setAttribute(EVertexAttribute::Position_0, 1, { 1, -1, 0});
	vertexBuffer.setAttribute(EVertexAttribute::Position_0, 2, { 0,  1, 0});

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
