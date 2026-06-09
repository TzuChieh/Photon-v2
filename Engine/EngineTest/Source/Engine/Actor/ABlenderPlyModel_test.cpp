#include <Engine/Actor/ABlenderPlyModel.h>
#include <Engine/Actor/Basic/exceptions.h>
#include <Engine/Actor/Geometry/GBlenderPlyPolygonMesh.h>
#include <Engine/Actor/Material/MatteOpaque.h>
#include <Engine/Core/HitDetail.h>
#include <Engine/Core/SurfaceHit.h>
#include <Engine/Core/Intersection/Primitive.h>
#include <Engine/Core/Intersection/PrimitiveMetadata.h>
#include <Engine/DataIO/FileSystem/Filesystem.h>
#include <Engine/DataIO/FileSystem/TProjectPath.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/CookingContext.h>
#include <Engine/World/Foundation/TransientVisualElement.h>

#include <gtest/gtest.h>

#include <memory>
#include <vector>

using namespace ph;
using namespace ph::math;

namespace
{

void write_material_range_blender_ply(
	const Path& plyFile,
	const std::vector<uint32>& triMatIds)
{
	const auto numTris = triMatIds.size();
	const auto numLoops = numTris * 3;

	GBlenderPlyPolygonMesh::SdlWritePly writePly;
	writePly.path = plyFile;
	writePly.rawVertPositions.reserve(numLoops * 3);
	writePly.rawVertLoopNormals.reserve(numLoops * 3);
	writePly.rawVertLoopUVs.reserve(numLoops * 2);
	writePly.vertPositionIndices.reserve(numLoops);
	writePly.vertLoopIndices.reserve(numLoops);
	writePly.triMatIds = triMatIds;

	// Some random synthetic data
	for(std::size_t loopIndex = 0; loopIndex < numLoops; ++loopIndex)
	{
		const auto triIndex = static_cast<float32>(loopIndex / 3);
		const auto triVertexIndex = loopIndex % 3;

		writePly.rawVertPositions.push_back(triIndex);
		writePly.rawVertPositions.push_back(triVertexIndex == 1 ? 1.0f : 0.0f);
		writePly.rawVertPositions.push_back(triVertexIndex == 2 ? 1.0f : 0.0f);

		writePly.rawVertLoopNormals.push_back(0.0f);
		writePly.rawVertLoopNormals.push_back(1.0f);
		writePly.rawVertLoopNormals.push_back(0.0f);

		writePly.rawVertLoopUVs.push_back(0.0f);
		writePly.rawVertLoopUVs.push_back(0.0f);

		writePly.vertPositionIndices.push_back(static_cast<uint32>(loopIndex));
		writePly.vertLoopIndices.push_back(static_cast<uint32>(loopIndex));
	}

	writePly();
}

std::vector<std::shared_ptr<Material>> make_material_slots(const std::size_t numSlots)
{
	std::vector<std::shared_ptr<Material>> materials;
	materials.reserve(numSlots);

	// Some random synthetic data
	for(std::size_t slotIndex = 0; slotIndex < numSlots; ++slotIndex)
	{
		auto material = std::make_shared<MatteOpaque>();
		material->setAlbedo(
			static_cast<real>(slotIndex + 1) / static_cast<real>(numSlots),
			0.5_r,
			0.25_r);
		materials.push_back(material);
	}

	return materials;
}

}// end namespace

TEST(ABlenderPlyModelTest, FaceHitResolvesMaterialSlotMetadata)
{
	const std::vector<uint32> faceMaterialSlots = {4, 7};
	const Path testDirectory = EngineTestIntermediatePath(
		"ABlenderPlyModelTest/FaceHitResolvesMaterialSlotMetadata");
	const Path tempPlyFile = testDirectory / "material-ranges.ply";
	Filesystem::remove(testDirectory, true);
	Filesystem::createDirectories(testDirectory);
	write_material_range_blender_ply(tempPlyFile, faceMaterialSlots);

	auto geometry = std::make_shared<GBlenderPlyPolygonMesh>();
	geometry->setPlyFile(tempPlyFile);

	ABlenderPlyModel actor;
	actor.setBaseTransform(TDecomposedTransform<real>());
	actor.setGeometry(geometry);
	actor.setMaterials(make_material_slots(16));

	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	const TransientVisualElement result = actor.stagelessCook(ctx);

	ASSERT_EQ(result.primitivesView.size(), 1);
	const Primitive* const primitive = result.primitivesView[0];
	ASSERT_NE(primitive, nullptr);

	// Face IDs should resolve to the original material slot IDs exported in the PLY.
	EXPECT_EQ(primitive->numMetadataSlots(), 16);
	for(uint64 faceID = 0; faceID < faceMaterialSlots.size(); ++faceID)
	{
		EXPECT_EQ(primitive->toMetadataSlot(faceID), faceMaterialSlots[faceID]);
	}

	HitDetail face0Detail;
	face0Detail.setHitIntrinsics(primitive, Vector3R(0, 0, 0), 1.0_r, 0);
	HitDetail face1Detail;
	face1Detail.setHitIntrinsics(primitive, Vector3R(0, 0, 0), 1.0_r, 1);

	const PrimitiveMetadata& face0Metadata = SurfaceHit::getPrimitiveMetadataRef(face0Detail);
	const PrimitiveMetadata& face1Metadata = SurfaceHit::getPrimitiveMetadataRef(face1Detail);

	EXPECT_EQ(&face0Metadata, &primitive->getMetadata(faceMaterialSlots[0]));
	EXPECT_EQ(&face1Metadata, &primitive->getMetadata(faceMaterialSlots[1]));
	EXPECT_NE(&face0Metadata, &face1Metadata);
}

TEST(ABlenderPlyModelTest, FaceMaterialSlotMapCanHaveDistinctCounts)
{
	const std::vector<uint32> faceMaterialSlots = {1, 1, 4, 4, 2, 4};
	const Path testDirectory = EngineTestIntermediatePath(
		"ABlenderPlyModelTest/FaceMaterialSlotMapCanHaveDistinctCounts");
	const Path tempPlyFile = testDirectory / "material-ranges.ply";
	Filesystem::remove(testDirectory, true);
	Filesystem::createDirectories(testDirectory);
	write_material_range_blender_ply(tempPlyFile, faceMaterialSlots);

	auto geometry = std::make_shared<GBlenderPlyPolygonMesh>();
	geometry->setPlyFile(tempPlyFile);

	ABlenderPlyModel actor;
	actor.setBaseTransform(TDecomposedTransform<real>());
	actor.setGeometry(geometry);
	actor.setMaterials(make_material_slots(8));

	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	const TransientVisualElement result = actor.stagelessCook(ctx);

	ASSERT_EQ(result.primitivesView.size(), 1);
	const Primitive* const primitive = result.primitivesView[0];
	ASSERT_NE(primitive, nullptr);

	// Slot count (8), unique slot IDs (3), face count (6), and range count (4) all differ.
	EXPECT_EQ(primitive->numMetadataSlots(), 8);
	for(uint64 faceID = 0; faceID < faceMaterialSlots.size(); ++faceID)
	{
		EXPECT_EQ(primitive->toMetadataSlot(faceID), faceMaterialSlots[faceID]);
	}
}

TEST(ABlenderPlyModelTest, ThrowsOnOutOfRangeMaterialSlot)
{
	const std::vector<uint32> faceMaterialSlots = {0, 8};// 8 is the 9-th, which is out of range for `actor`
	const Path testDirectory = EngineTestIntermediatePath(
		"ABlenderPlyModelTest/ThrowsOnOutOfRangeMaterialSlot");
	const Path tempPlyFile = testDirectory / "out-of-range-material-slot.ply";
	Filesystem::remove(testDirectory, true);
	Filesystem::createDirectories(testDirectory);
	write_material_range_blender_ply(tempPlyFile, faceMaterialSlots);

	auto geometry = std::make_shared<GBlenderPlyPolygonMesh>();
	geometry->setPlyFile(tempPlyFile);

	ABlenderPlyModel actor;
	actor.setBaseTransform(TDecomposedTransform<real>());
	actor.setGeometry(geometry);
	actor.setMaterials(make_material_slots(8));

	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	EXPECT_THROW(actor.stagelessCook(ctx), ActorCookException);
}
