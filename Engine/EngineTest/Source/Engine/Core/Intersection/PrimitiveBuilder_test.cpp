#include <Engine/Core/Intersection/PrimitiveBuilder.h>
#include <Engine/Core/Intersection/PEmpty.h>
#include <Engine/Core/Intersection/PrimitiveMetadata.h>
#include <Engine/Core/Intersection/DataStructure/TIndexRangeMap.h>

#include <gtest/gtest.h>

#include <memory>
#include <vector>

using namespace ph;

namespace
{

auto make_metadata_refs(
	const PrimitiveMetadata* const metadata0,
	const PrimitiveMetadata* const metadata1,
	const PrimitiveMetadata* const metadata2)
{
	auto metadatas = std::make_unique<const PrimitiveMetadata*[]>(3);
	metadatas[0] = metadata0;
	metadatas[1] = metadata1;
	metadatas[2] = metadata2;
	return metadatas;
}

}// end namespace

TEST(PrimitiveBuilderTest, BuildsFromReferencedAndEmbeddedPrimitives)
{
	PrimitiveMetadata metadata;

	{
		PEmpty primitive;

		auto builtPrimitive = PrimitiveBuilder::referencing(&primitive)
			.injectMetadata(&metadata)
			.build();

		EXPECT_EQ(&builtPrimitive.getInjectee(), &primitive);
		EXPECT_EQ(&builtPrimitive.getMetadata(0), &metadata);
	}

	{
		auto builtPrimitive = PrimitiveBuilder::embedding<PEmpty>()
			.injectMetadata(&metadata)
			.build();

		EXPECT_EQ(&builtPrimitive.getMetadata(0), &metadata);
	}
}

TEST(PrimitiveBuilderTest, InjectsOwnedAndMappedMetadata)
{
	{
		PrimitiveMetadata metadata;

		auto builtPrimitive = PrimitiveBuilder::embedding<PEmpty>()
			.injectMetadataCopy(metadata)
			.build();

		EXPECT_NE(&builtPrimitive.getMetadata(0), &metadata);
	}

	{
		PrimitiveMetadata metadata0;
		PrimitiveMetadata metadata1;
		PrimitiveMetadata metadata2;

		TIndexRangeMap<uint64, uint32> faceIdToMetadataSlot(2);
		faceIdToMetadataSlot.setRangeMap(0, 5, 2);
		faceIdToMetadataSlot.setRangeMap(1, 10, 0);

		std::vector<uint64> maxFaceIds;
		std::vector<uint32> materialSlots;
		faceIdToMetadataSlot.forEachEntry(
			[&maxFaceIds, &materialSlots](const uint64 maxFaceId, const uint32 materialSlot)
			{
				maxFaceIds.push_back(maxFaceId);
				materialSlots.push_back(materialSlot);
			});
		EXPECT_EQ(maxFaceIds, std::vector<uint64>({5, 10}));
		EXPECT_EQ(materialSlots, std::vector<uint32>({2, 0}));

		auto builtPrimitive = PrimitiveBuilder::embedding<PEmpty>()
			.injectMetadataArray(
				make_metadata_refs(&metadata0, &metadata1, &metadata2),
				3,
				faceIdToMetadataSlot)
			.build();

		EXPECT_EQ(builtPrimitive.numMetadataSlots(), 3);
		EXPECT_EQ(builtPrimitive.toMetadataSlot(4), 2);
		EXPECT_EQ(builtPrimitive.toMetadataSlot(8), 0);
		EXPECT_EQ(&builtPrimitive.getMetadata(builtPrimitive.toMetadataSlot(4)), &metadata2);
		EXPECT_EQ(&builtPrimitive.getMetadata(builtPrimitive.toMetadataSlot(8)), &metadata0);
	}
}
