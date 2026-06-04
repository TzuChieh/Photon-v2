#include <Engine/Actor/Geometry/GBlenderPlyPolygonMesh.h>
#include <Engine/Core/Intersection/DataStructure/TIndexedPolygonBuffer.h>
#include <Engine/DataIO/FileSystem/Filesystem.h>
#include <Engine/DataIO/FileSystem/TProjectPath.h>
#include <Engine/DataIO/PlyFile.h>
#include <Engine/Math/math.h>
#include <Engine/World/Foundation/CookedGeometry.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/CookingContext.h>

#include <gtest/gtest.h>

#include <array>

using namespace ph;
using namespace ph::math;

namespace
{

class TestableGBlenderPlyPolygonMesh final : public GBlenderPlyPolygonMesh
{
public:
	using GBlenderPlyPolygonMesh::loadTriangleBuffer;
};

void write_split_index_blender_ply(const Path& plyFile)
{
	// Quad: 4 vertices (which need 6 indices for 2 triangles)
	GBlenderPlyPolygonMesh::SdlWritePly writePly;
	writePly.path = plyFile;
	writePly.rawVertPositions = {
		10, 0, 0,
		20, 0, 0,
		30, 0, 0,
		40, 0, 0};
	writePly.rawVertLoopNormals = {
		 1, 0, 0,
		 0, 1, 0,
		 0, 0, 1,
		-1, 0, 0};
	writePly.rawVertLoopUVs = {
		0, 0,
		1, 0,
		1, 1,
		0, 1};

	// Position and loop indices are paired per triangle corner.
	writePly.vertPositionIndices = {2, 0, 3, 2, 3, 1};
	writePly.vertLoopIndices = {0, 1, 2, 0, 2, 3};
	writePly.triMatIds = {4, 7};
	writePly();
}

}// end anonymous namespace

TEST(GBlenderPlyPolygonMeshTest, LoadTriangleBuffer)
{
	constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-6_r;

	const Path testDirectory = EngineTestIntermediatePath(
		"GBlenderPlyPolygonMeshTest/LoadTriangleBuffer");
	const Path tempPlyFile = testDirectory / "split-indices.ply";
	Filesystem::remove(testDirectory, true);
	Filesystem::createDirectories(testDirectory);
	write_split_index_blender_ply(tempPlyFile);

	PlyFile plyFile(tempPlyFile);
	const IndexedTriangleBuffer buffer = TestableGBlenderPlyPolygonMesh().loadTriangleBuffer(plyFile);
	const auto& vertexBuffer = buffer.getVertexBuffer();
	const auto& indexBuffer = buffer.getIndexBuffer();

	EXPECT_EQ(buffer.numFaces(), 2);
	ASSERT_EQ(vertexBuffer.numVertices(), 4);
	ASSERT_EQ(indexBuffer.numUInts(), 6);

	// The cooked buffer unifies split Blender indices into one loop-indexed vertex buffer.
	const std::array<Vector3R, 4> expectedPositions = {{
		{30, 0, 0},
		{10, 0, 0},
		{40, 0, 0},
		{20, 0, 0}}};
	const std::array<Vector3R, 4> expectedNormals = {{
		{ 1, 0, 0},
		{ 0, 1, 0},
		{ 0, 0, 1},
		{-1, 0, 0}}};
	const std::array<Vector3R, 4> expectedTexCoords = {{
		{0, 0, 0},
		{1, 0, 0},
		{1, 1, 0},
		{0, 1, 0}}};

	for(std::size_t vi = 0; vi < vertexBuffer.numVertices(); ++vi)
	{
		EXPECT_TRUE(vertexBuffer.getAttribute(EVertexAttribute::Position_0, vi).isNear(
			expectedPositions[vi], MAX_ALLOWED_ABS_ERROR));
		EXPECT_TRUE(vertexBuffer.getAttribute(EVertexAttribute::Normal_0, vi).isNear(
			expectedNormals[vi], MAX_ALLOWED_ABS_ERROR));
		EXPECT_TRUE(vertexBuffer.getAttribute(EVertexAttribute::TexCoord_0, vi).isNear(
			expectedTexCoords[vi], MAX_ALLOWED_ABS_ERROR));
	}

	const std::array<uint64, 6> expectedIndices = {0, 1, 2, 0, 2, 3};
	for(std::size_t ii = 0; ii < indexBuffer.numUInts(); ++ii)
	{
		EXPECT_EQ(indexBuffer.getUInt(ii), expectedIndices[ii]);
	}

}

TEST(GBlenderPlyPolygonMeshTest, StoreCooked)
{
	const Path testDirectory = EngineTestIntermediatePath(
		"GBlenderPlyPolygonMeshTest/StoreCooked");
	const Path tempPlyFile = testDirectory / "split-indices.ply";
	Filesystem::remove(testDirectory, true);
	Filesystem::createDirectories(testDirectory);
	write_split_index_blender_ply(tempPlyFile);

	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	TestableGBlenderPlyPolygonMesh mesh;
	mesh.setPlyFile(tempPlyFile);

	const CookedGeometry* cooked = mesh.createCooked(ctx);

	ASSERT_NE(cooked, nullptr);
	ASSERT_NE(cooked->triangleView, nullptr);
	EXPECT_EQ(cooked->primitives.size(), 1);
	EXPECT_EQ(cooked->triangleView->numFaces(), 2);
	EXPECT_EQ(cooked->triangleView->getVertexBuffer().numVertices(), 4);
	EXPECT_EQ(cooked->triangleView->getIndexBuffer().numUInts(), 6);
	EXPECT_EQ(cooked->faceIdToMetadataSlot.get(0), 4);
	EXPECT_EQ(cooked->faceIdToMetadataSlot.get(1), 7);
}
