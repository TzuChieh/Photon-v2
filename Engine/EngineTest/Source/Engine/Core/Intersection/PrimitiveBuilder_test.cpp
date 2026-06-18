#include <Engine/Core/Intersection/IntersectableBuilder.h>
#include <Engine/Core/Intersection/PrimitiveBuilder.h>
#include <Engine/Core/Intersection/PEmpty.h>
#include <Engine/Core/Intersection/PrimitiveMetadata.h>
#include <Engine/Core/Intersection/TTransformedIntersectable.h>
#include <Engine/Core/Intersection/TTransformedPrimitive.h>
#include <Engine/Core/Intersection/DataStructure/TIndexRangeMap.h>
#include <Engine/Core/Transform/StaticAffineTransform.h>
#include <Engine/Core/Transform/StaticRigidTransform.h>

#include <gtest/gtest.h>

#include <memory>
#include <type_traits>
#include <vector>

using namespace ph;

TEST(PrimitiveBuilderTest, BuildsFromReferencedAndEmbeddedPrimitives)
{
	{
		auto builtPrimitive = PrimitiveBuilder::embedding<PEmpty>()
			.build();

		static_assert(std::is_same_v<decltype(builtPrimitive), PEmpty>);
		EXPECT_EQ(builtPrimitive.numMetadataSlots(), 1);
	}

	{
		PEmpty primitive;
		PrimitiveMetadata metadata;

		auto builtPrimitive = PrimitiveBuilder::referencing(&primitive)
			.injectMetadata(&metadata)
			.build();

		static_assert(std::is_same_v<
			decltype(builtPrimitive),
			TMetaInjectionPrimitive<
				ReferencedPrimitiveMetadataGetter,
				TReferencedPrimitiveGetter<PEmpty>>>);

		EXPECT_EQ(&builtPrimitive.getInjectee(), &primitive);
		EXPECT_EQ(&builtPrimitive.getMetadata(0), &metadata);
	}

	{
		PrimitiveMetadata metadata;

		auto builtPrimitive = PrimitiveBuilder::embedding<PEmpty>()
			.injectMetadata(&metadata)
			.build();

		static_assert(std::is_same_v<
			decltype(builtPrimitive),
			TMetaInjectionPrimitive<
				ReferencedPrimitiveMetadataGetter,
				TEmbeddedPrimitiveGetter<PEmpty>>>);

		EXPECT_EQ(&builtPrimitive.getMetadata(0), &metadata);
	}
}

TEST(PrimitiveBuilderTest, BuildsRigidTransformedPrimitives)
{
	PEmpty primitive;
	StaticRigidTransform localToWorld;
	StaticRigidTransform worldToLocal;

	auto builtPrimitive = PrimitiveBuilder::referencing(&primitive)
		.rigidTransform(&localToWorld, &worldToLocal)
		.build();

	static_assert(std::is_same_v<
		decltype(builtPrimitive),
		TTransformedPrimitive<TReferencedPrimitiveGetter<PEmpty>>>);

	EXPECT_EQ(builtPrimitive.numMetadataSlots(), primitive.numMetadataSlots());
}

TEST(PrimitiveBuilderTest, BuildsRigidTransformedPrimitivesAfterMetadataInjection)
{
	PEmpty primitive;
	PrimitiveMetadata metadata;
	StaticRigidTransform localToWorld;
	StaticRigidTransform worldToLocal;

	auto builtPrimitive = PrimitiveBuilder::referencing(&primitive)
		.injectMetadata(&metadata)
		.rigidTransform(&localToWorld, &worldToLocal)
		.build();

	using InjectedPrimitive = TMetaInjectionPrimitive<
		ReferencedPrimitiveMetadataGetter,
		TReferencedPrimitiveGetter<PEmpty>>;

	static_assert(std::is_same_v<
		decltype(builtPrimitive),
		TTransformedPrimitive<TEmbeddedPrimitiveGetter<InjectedPrimitive>>>);

	EXPECT_EQ(builtPrimitive.numMetadataSlots(), 1);
	EXPECT_EQ(&builtPrimitive.getMetadata(0), &metadata);
}

TEST(PrimitiveBuilderTest, DecaysToIntersectableForGeneralTransform)
{
	PEmpty primitive;
	StaticAffineTransform localToWorld;
	StaticAffineTransform worldToLocal;

	auto builtIntersectable = PrimitiveBuilder::referencing(&primitive)
		.transform(&localToWorld, &worldToLocal)
		.build();

	static_assert(std::is_same_v<
		decltype(builtIntersectable),
		TTransformedIntersectable<TReferencedPrimitiveGetter<PEmpty>>>);

	EXPECT_FALSE(builtIntersectable.mayOverlapVolume(math::AABB3D(math::Vector3R(0))));
}

TEST(PrimitiveBuilderTest, DecaysToIntersectableAfterMetadataInjection)
{
	PEmpty primitive;
	PrimitiveMetadata metadata;
	StaticAffineTransform localToWorld;
	StaticAffineTransform worldToLocal;

	auto builtIntersectable = PrimitiveBuilder::referencing(&primitive)
		.injectMetadata(&metadata)
		.transform(&localToWorld, &worldToLocal)
		.build();

	using InjectedPrimitive = TMetaInjectionPrimitive<
		ReferencedPrimitiveMetadataGetter,
		TReferencedPrimitiveGetter<PEmpty>>;

	static_assert(std::is_same_v<
		decltype(builtIntersectable),
		TTransformedIntersectable<TEmbeddedPrimitiveGetter<InjectedPrimitive>>>);

	EXPECT_FALSE(builtIntersectable.mayOverlapVolume(math::AABB3D(math::Vector3R(0))));
}

TEST(PrimitiveBuilderTest, BuildsTransformedIntersectables)
{
	{
		PEmpty referencedPrimitive;
		StaticAffineTransform localToWorld;
		StaticAffineTransform worldToLocal;

		auto referencedIntersectable = IntersectableBuilder::referencing(&referencedPrimitive)
			.transform(&localToWorld, &worldToLocal)
			.build();

		static_assert(std::is_same_v<
			decltype(referencedIntersectable),
			TTransformedIntersectable<TReferencedIntersectableGetter<PEmpty>>>);

		EXPECT_FALSE(referencedIntersectable.mayOverlapVolume(math::AABB3D(math::Vector3R(0))));
	}

	{
		StaticAffineTransform localToWorld;
		StaticAffineTransform worldToLocal;

		auto embeddedIntersectable = IntersectableBuilder::embedding<PEmpty>()
			.transform(&localToWorld, &worldToLocal)
			.build();

		static_assert(std::is_same_v<
			decltype(embeddedIntersectable),
			TTransformedIntersectable<TEmbeddedIntersectableGetter<PEmpty>>>);

		EXPECT_FALSE(embeddedIntersectable.mayOverlapVolume(math::AABB3D(math::Vector3R(0))));
	}
}

TEST(PrimitiveBuilderTest, InjectsOwnedAndMappedMetadata)
{
	{
		PrimitiveMetadata metadata;

		auto builtPrimitive = PrimitiveBuilder::embedding<PEmpty>()
			.injectMetadataCopy(metadata)
			.build();

		EXPECT_EQ(builtPrimitive.numMetadataSlots(), 1);
		EXPECT_NE(&builtPrimitive.getMetadata(0), &metadata);
	}

	{
		PrimitiveMetadata metadata0;
		PrimitiveMetadata metadata1;
		PrimitiveMetadata metadata2;
		auto metadatas = std::make_unique<const PrimitiveMetadata*[]>(3);
		metadatas[0] = &metadata0;
		metadatas[1] = &metadata1;
		metadatas[2] = &metadata2;

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
				std::move(metadatas),
				3,
				&faceIdToMetadataSlot)
			.build();

		EXPECT_EQ(builtPrimitive.numMetadataSlots(), 3);
		EXPECT_EQ(&builtPrimitive.getMetadata(0), &metadata0);
		EXPECT_EQ(&builtPrimitive.getMetadata(1), &metadata1);
		EXPECT_EQ(&builtPrimitive.getMetadata(2), &metadata2);
		EXPECT_EQ(builtPrimitive.toMetadataSlot(4), 2);
		EXPECT_EQ(builtPrimitive.toMetadataSlot(8), 0);
		EXPECT_EQ(&builtPrimitive.getMetadata(builtPrimitive.toMetadataSlot(4)), &metadata2);
		EXPECT_EQ(&builtPrimitive.getMetadata(builtPrimitive.toMetadataSlot(8)), &metadata0);
	}
}
