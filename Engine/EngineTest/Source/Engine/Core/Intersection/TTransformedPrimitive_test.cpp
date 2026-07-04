#include <Engine/Core/Intersection/PrimitiveBuilder.h>
#include <Engine/Core/Intersection/PEmpty.h>
#include <Engine/Core/Intersection/PLatLong01Sphere.h>
#include <Engine/Core/Intersection/PrimitiveMetadata.h>
#include <Engine/Core/Intersection/TTransformedPrimitive.h>
#include <Engine/Core/Intersection/DataStructure/TIndexRangeMap.h>
#include <Engine/Core/Transform/StaticRigidTransform.h>
#include <Engine/Core/Transform/DynamicLinearTranslation.h>

#include <gtest/gtest.h>

#include <memory>
#include <type_traits>

using namespace ph;

TEST(TTransformedPrimitiveTest, UsesReferencedGetter)
{
	static_assert(std::is_constructible_v<
		TTransformedPrimitive<TReferencedPrimitiveGetter<Primitive>>,
		TReferencedPrimitiveGetter<Primitive>,
		const RigidTransform*,
		const RigidTransform*>);

	PEmpty primitive;
	StaticRigidTransform localToWorld;
	StaticRigidTransform worldToLocal;

	TTransformedPrimitive<TReferencedPrimitiveGetter<Primitive>> transformedPrimitive(
		TReferencedPrimitiveGetter<Primitive>(&primitive),
		&localToWorld,
		&worldToLocal);

	EXPECT_EQ(transformedPrimitive.numMetadataSlots(), primitive.numMetadataSlots());
}

TEST(TTransformedPrimitiveTest, ForwardsMetadataSlots)
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

	auto builtPrimitive = PrimitiveBuilder::embedding<PEmpty>()
		.injectMetadataArray(
			std::move(metadatas),
			3,
			&faceIdToMetadataSlot)
		.build();

	StaticRigidTransform localToWorld;
	StaticRigidTransform worldToLocal;
	TTransformedPrimitive<TReferencedPrimitiveGetter<Primitive>> transformedPrimitive(
		TReferencedPrimitiveGetter<Primitive>(&builtPrimitive),
		&localToWorld,
		&worldToLocal);

	EXPECT_EQ(transformedPrimitive.numMetadataSlots(), 3);
	EXPECT_EQ(transformedPrimitive.toMetadataSlot(4), 2);
	EXPECT_EQ(transformedPrimitive.toMetadataSlot(8), 0);
	EXPECT_EQ(&transformedPrimitive.getMetadata(transformedPrimitive.toMetadataSlot(4)), &metadata2);
	EXPECT_EQ(&transformedPrimitive.getMetadata(transformedPrimitive.toMetadataSlot(8)), &metadata0);
}

TEST(TTransformedPrimitiveTest, LinearTranslationAabbBoundsCookedStep)
{
	PLatLong01Sphere sphere(1);
	DynamicLinearTranslation localToWorld({-2, 0, 0}, {3, 0, 0});
	DynamicLinearTranslation worldToLocal = localToWorld.makeInversed();

	TTransformedPrimitive<TReferencedPrimitiveGetter<Primitive>> transformedPrimitive(
		TReferencedPrimitiveGetter<Primitive>(&sphere),
		&localToWorld,
		&worldToLocal);

	const math::AABB3D aabb = transformedPrimitive.calcAABB();

	EXPECT_LE(aabb.getMinVertex().x(), -3.0_r);
	EXPECT_LE(aabb.getMinVertex().y(), -1.0_r);
	EXPECT_LE(aabb.getMinVertex().z(), -1.0_r);
	EXPECT_GE(aabb.getMaxVertex().x(),  4.0_r);
	EXPECT_GE(aabb.getMaxVertex().y(),  1.0_r);
	EXPECT_GE(aabb.getMaxVertex().z(),  1.0_r);
	EXPECT_TRUE(transformedPrimitive.mayOverlapVolume(
		math::AABB3D(math::Vector3R(3.5_r, 0, 0))));
}
