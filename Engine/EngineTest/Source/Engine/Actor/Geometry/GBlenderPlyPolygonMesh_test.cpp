#include <Engine/Actor/Geometry/GBlenderPlyPolygonMesh.h>
#include <Engine/Core/Intersection/DataStructure/TIndexedPolygonBuffer.h>
#include <Engine/Core/Transform/StaticAffineTransform.h>
#include <Engine/DataIO/FileSystem/Filesystem.h>
#include <Engine/DataIO/FileSystem/TProjectPath.h>
#include <Engine/DataIO/PlyFile.h>
#include <Engine/Math/math.h>
#include <Engine/Math/TDecomposedTransform.h>
#include <Engine/SDL/TSdl.h>
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
	writePly.triCustom2 = {0.5f, 0.125f};
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
	const auto& attributeBuffer = buffer.getAttributeBuffer();
	const auto& indexBuffer = buffer.getIndexBuffer();

	EXPECT_EQ(buffer.numFaces(), 2);
	ASSERT_EQ(indexBuffer.numUInts(), 6);
	EXPECT_FALSE(attributeBuffer.hasAttribute(EPrimitiveAttribute::Tangent_0));
	EXPECT_FALSE(attributeBuffer.hasAttribute(EPrimitiveAttribute::MikkTSpaceTangent_0));
	EXPECT_EQ(
		attributeBuffer.getAttribute(EPrimitiveAttribute::Custom_2, 1),
		Vector3R(0.125_r, 0, 0));

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

	for(std::size_t vi = 0; vi < expectedPositions.size(); ++vi)
	{
		EXPECT_TRUE(attributeBuffer.getAttribute(EPrimitiveAttribute::Position_0, vi).isNear(
			expectedPositions[vi], MAX_ALLOWED_ABS_ERROR));
		EXPECT_TRUE(attributeBuffer.getAttribute(EPrimitiveAttribute::Normal_0, vi).isNear(
			expectedNormals[vi], MAX_ALLOWED_ABS_ERROR));
		EXPECT_TRUE(attributeBuffer.getAttribute(EPrimitiveAttribute::TexCoord_0, vi).isNear(
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

	auto mesh = TSdl<GBlenderPlyPolygonMesh>::makeResource();
	mesh->setPlyFile(tempPlyFile);

	mesh->cook(ctx, *resources.makeGeometry(ctx.getKey(mesh)));
	const CookedGeometry* cooked = ctx.getCooked(mesh);

	ASSERT_NE(cooked, nullptr);
	ASSERT_NE(cooked->triangleView, nullptr);
	EXPECT_EQ(cooked->primitives.size(), 1);
	EXPECT_EQ(cooked->triangleView->numFaces(), 2);
	EXPECT_EQ(cooked->triangleView->getIndexBuffer().numUInts(), 6);
	EXPECT_EQ(cooked->faceIdToMetadataSlot.get(0), 4);
	EXPECT_EQ(cooked->faceIdToMetadataSlot.get(1), 7);
}

TEST(GBlenderPlyPolygonMeshTest, LoadsTangents)
{
	const Path testDirectory = EngineTestIntermediatePath("GBlenderPlyPolygonMeshTest/LoadsTangents");
	Filesystem::remove(testDirectory, true);
	Filesystem::createDirectories(testDirectory);

	GBlenderPlyPolygonMesh::SdlWritePly writePly;
	writePly.path = testDirectory / "tangents.ply";
	writePly.rawVertPositions = {0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0};
	writePly.rawVertLoopNormals = {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1};
	writePly.rawVertLoopTangents = {
		 1,  0, 0,  1,
		 0,  1, 0,  1,
		-1,  0, 0,  1,
		 1,  0, 0, -1,
		 0,  1, 0, -1,
		-1,  0, 0, -1};
	writePly.rawVertLoopUVs = {0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1};
	writePly.vertPositionIndices = {0, 1, 2, 0, 2, 3};
	writePly.vertLoopIndices = {0, 1, 2, 3, 4, 5};
	writePly.triMatIds = {0, 0};
	writePly.triCustom2 = {0.25f, 0.75f};
	writePly();

	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);
	
	GBlenderPlyPolygonMesh mesh;
	mesh.setPlyFile(writePly.path);

	{
		CookedGeometry cooked;
		mesh.storeCooked(ctx, cooked);
		ASSERT_NE(cooked.triangleView, nullptr);
		const auto& buffer = *cooked.triangleView;
		ASSERT_TRUE(buffer.hasAttribute(EPrimitiveAttribute::MikkTSpaceTangent_0));

		std::array<uint32, 3> signBits;
		const auto tangents = buffer.getFaceVertexAttributes(
			EPrimitiveAttribute::MikkTSpaceTangent_0, 0, &signBits);
		EXPECT_TRUE(tangents[0].isNear(Vector3R(1, 0, 0), 1e-4_r));
		EXPECT_TRUE(tangents[1].isNear(Vector3R(0, 1, 0), 1e-4_r));
		EXPECT_TRUE(tangents[2].isNear(Vector3R(-1, 0, 0), 1e-4_r));
		EXPECT_EQ(signBits, (std::array<uint32, 3>{0, 0, 0}));

		buffer.getFaceVertexAttributes(EPrimitiveAttribute::MikkTSpaceTangent_0, 1, &signBits);
		EXPECT_EQ(signBits, (std::array<uint32, 3>{1, 1, 1}));
		EXPECT_EQ(buffer.getAttributeBuffer().getAttribute(EPrimitiveAttribute::Custom_2, 1), Vector3R(0.75_r, 0, 0));
	}

	// A baked reflection flips the first corner's tangent and handedness.
	{
		TDecomposedTransform<real> transformData;
		transformData.scale(-1, 1, 1);
		const StaticAffineTransform reflection = StaticAffineTransform::makeForward(transformData);

		CookedGeometry cooked;
		mesh.storeCookedWithBakedTransform(ctx, reflection, cooked);
		ASSERT_NE(cooked.triangleView, nullptr);

		std::array<uint32, 3> signBits;
		const auto tangents = cooked.triangleView->getFaceVertexAttributes(
			EPrimitiveAttribute::MikkTSpaceTangent_0, 0, &signBits);
		EXPECT_TRUE(tangents[0].isNear(Vector3R(-1, 0, 0), 1e-4_r));
		EXPECT_EQ(signBits[0], 1u);
	}
}
