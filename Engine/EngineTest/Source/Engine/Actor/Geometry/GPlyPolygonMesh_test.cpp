#include <Engine/Actor/Geometry/GPlyPolygonMesh.h>
#include <Engine/Core/Intersection/DataStructure/TIndexedPolygonBuffer.h>
#include <Engine/Core/Transform/StaticAffineTransform.h>
#include <Engine/DataIO/FileSystem/Filesystem.h>
#include <Engine/DataIO/FileSystem/TProjectPath.h>
#include <Engine/DataIO/Stream/FormattedTextOutputStream.h>
#include <Engine/Math/TDecomposedTransform.h>
#include <Engine/World/Foundation/CookedGeometry.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/CookingContext.h>

#include <gtest/gtest.h>

#include <array>

using namespace ph;
using namespace ph::math;

TEST(GPlyPolygonMeshTest, LoadsTangents)
{
	const Path testDirectory = EngineTestIntermediatePath("GPlyPolygonMeshTest/LoadsTangents");
	Filesystem::remove(testDirectory, true);
	Filesystem::createDirectories(testDirectory);

	TDecomposedTransform<real> reflectionData;
	reflectionData.scale(-1, 1, 1);
	const StaticAffineTransform reflection = StaticAffineTransform::makeForward(reflectionData);

	// Tangent without handedness
	{
		const Path plyFile = testDirectory / "tangent.ply";
		{
			FormattedTextOutputStream stream(plyFile);
			stream.writeString(
				"ply\n"
				"format ascii 1.0\n"
				"element vertex 3\n"
				"property float x\n"
				"property float y\n"
				"property float z\n"
				"property float nx\n"
				"property float ny\n"
				"property float nz\n"
				"property float tx\n"
				"property float ty\n"
				"property float tz\n"
				"element face 1\n"
				"property list uchar int vertex_indices\n"
				"end_header\n"
				// position  normal  tangent
				"  0 0 0     0 0 1    1 0 0\n"
				"  1 0 0     0 0 1    0 1 0\n"
				"  0 1 0     0 0 1   -1 0 0\n"
				"3 2 0 1\n");
		}

		CookedResourceCollection resources;
		CookingContext ctx(&resources, nullptr);
		CookedGeometry cooked;

		GPlyPolygonMesh mesh;
		mesh.setPlyFile(plyFile);
		mesh.storeCooked(ctx, cooked);

		ASSERT_NE(cooked.triangleView, nullptr);
		const auto& attributes = cooked.triangleView->getAttributeBuffer();
		ASSERT_TRUE(attributes.hasAttribute(EPrimitiveAttribute::Tangent_0));
		EXPECT_FALSE(attributes.hasAttribute(EPrimitiveAttribute::MikkTSpaceTangent_0));

		const auto tangents = cooked.triangleView->getFaceVertexAttributes(
			EPrimitiveAttribute::Tangent_0, 0);
		EXPECT_TRUE(tangents[0].isNear(Vector3R(-1, 0, 0), 1e-4_r));
		EXPECT_TRUE(tangents[1].isNear(Vector3R(1, 0, 0), 1e-4_r));
		EXPECT_TRUE(tangents[2].isNear(Vector3R(0, 1, 0), 1e-4_r));

		// Flipping two axes preserves handedness.
		{
			TDecomposedTransform<real> transformData;
			transformData.scale(-1, -1, 1);
			const StaticAffineTransform transform = StaticAffineTransform::makeForward(transformData);
			
			CookedGeometry baked;
			mesh.storeCookedWithBakedTransform(ctx, transform, baked);

			ASSERT_NE(baked.triangleView, nullptr);
			ASSERT_TRUE(baked.triangleView->hasAttribute(EPrimitiveAttribute::Tangent_0));
			EXPECT_FALSE(baked.triangleView->hasAttribute(EPrimitiveAttribute::MikkTSpaceTangent_0));
			const auto bakedTangents = baked.triangleView->getFaceVertexAttributes(
				EPrimitiveAttribute::Tangent_0, 0);
			EXPECT_TRUE(bakedTangents[0].isNear(Vector3R(1, 0, 0), 1e-4_r));
			EXPECT_TRUE(bakedTangents[1].isNear(Vector3R(-1, 0, 0), 1e-4_r));
			EXPECT_TRUE(bakedTangents[2].isNear(Vector3R(0, -1, 0), 1e-4_r));
		}

		// A reflection promotes the implicit positive sign to an explicit negative sign.
		{
			CookedGeometry baked;
			mesh.storeCookedWithBakedTransform(ctx, reflection, baked);

			ASSERT_NE(baked.triangleView, nullptr);
			EXPECT_FALSE(baked.triangleView->hasAttribute(EPrimitiveAttribute::Tangent_0));

			// Currently we use this attribute to carry sign bit
			ASSERT_TRUE(baked.triangleView->hasAttribute(EPrimitiveAttribute::MikkTSpaceTangent_0));

			std::array<uint32, 3> signBits;
			const auto bakedTangents = baked.triangleView->getFaceVertexAttributes(
				EPrimitiveAttribute::MikkTSpaceTangent_0, 0, &signBits);
			EXPECT_TRUE(bakedTangents[0].isNear(Vector3R(1, 0, 0), 1e-4_r));
			EXPECT_TRUE(bakedTangents[1].isNear(Vector3R(-1, 0, 0), 1e-4_r));
			EXPECT_TRUE(bakedTangents[2].isNear(Vector3R(0, 1, 0), 1e-4_r));
			EXPECT_EQ(signBits, (std::array<uint32, 3>{1, 1, 1}));
		}
	}

	// Tangent with handedness
	{
		const Path plyFile = testDirectory / "mikk-tangent.ply";
		{
			FormattedTextOutputStream stream(plyFile);
			stream.writeString(
				"ply\n"
				"format ascii 1.0\n"
				"element vertex 3\n"
				"property float x\n"
				"property float y\n"
				"property float z\n"
				"property float nx\n"
				"property float ny\n"
				"property float nz\n"
				"property float tx\n"
				"property float ty\n"
				"property float tz\n"
				"property float tw\n"
				"element face 1\n"
				"property list uchar int vertex_indices\n"
				"end_header\n"
				// position  normal  tangent  sign
				"  0 0 0     0 0 1    1 0 0    2\n"
				"  1 0 0     0 0 1    0 1 0   -0.5\n"
				"  0 1 0     0 0 1   -1 0 0    1\n"
				"3 2 0 1\n");
		}

		CookedResourceCollection resources;
		CookingContext ctx(&resources, nullptr);
		CookedGeometry cooked;

		GPlyPolygonMesh mesh;
		mesh.setPlyFile(plyFile);
		mesh.storeCooked(ctx, cooked);

		ASSERT_NE(cooked.triangleView, nullptr);
		const auto& attributes = cooked.triangleView->getAttributeBuffer();
		EXPECT_FALSE(attributes.hasAttribute(EPrimitiveAttribute::Tangent_0));
		ASSERT_TRUE(attributes.hasAttribute(EPrimitiveAttribute::MikkTSpaceTangent_0));

		std::array<uint32, 3> signBits;
		const auto tangents = cooked.triangleView->getFaceVertexAttributes(
			EPrimitiveAttribute::MikkTSpaceTangent_0, 0, &signBits);
		EXPECT_TRUE(tangents[0].isNear(Vector3R(-1, 0, 0), 1e-4_r));
		EXPECT_TRUE(tangents[1].isNear(Vector3R(1, 0, 0), 1e-4_r));
		EXPECT_TRUE(tangents[2].isNear(Vector3R(0, 1, 0), 1e-4_r));
		EXPECT_EQ(signBits, (std::array<uint32, 3>{0, 0, 1}));

		// A reflection flips existing signs once.
		{
			CookedGeometry baked;
			mesh.storeCookedWithBakedTransform(ctx, reflection, baked);

			ASSERT_NE(baked.triangleView, nullptr);
			EXPECT_FALSE(baked.triangleView->hasAttribute(EPrimitiveAttribute::Tangent_0));
			ASSERT_TRUE(baked.triangleView->hasAttribute(EPrimitiveAttribute::MikkTSpaceTangent_0));
			std::array<uint32, 3> bakedSignBits;
			const auto bakedTangents = baked.triangleView->getFaceVertexAttributes(
				EPrimitiveAttribute::MikkTSpaceTangent_0, 0, &bakedSignBits);
			EXPECT_TRUE(bakedTangents[0].isNear(Vector3R(1, 0, 0), 1e-4_r));
			EXPECT_TRUE(bakedTangents[1].isNear(Vector3R(-1, 0, 0), 1e-4_r));
			EXPECT_TRUE(bakedTangents[2].isNear(Vector3R(0, 1, 0), 1e-4_r));
			EXPECT_EQ(bakedSignBits, (std::array<uint32, 3>{1, 1, 0}));
		}
	}
}
