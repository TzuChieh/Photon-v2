#include <Engine/Core/Intersection/DataStructure/PrimitiveMetadataSlotMap.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(PrimitiveMetadataSlotMapTest, Behavior)
{
	PrimitiveMetadataSlotMap slotMap;
	slotMap.addSlot(0, 10);
	slotMap.addSlot(1, 20);
	slotMap.addSlot(2, 30);

	EXPECT_EQ(slotMap.getSlot(0), 0);
	EXPECT_EQ(slotMap.getSlot(5), 0);
	EXPECT_EQ(slotMap.getSlot(10), 0);

	EXPECT_EQ(slotMap.getSlot(11), 1);
	EXPECT_EQ(slotMap.getSlot(15), 1);
	EXPECT_EQ(slotMap.getSlot(20), 1);

	EXPECT_EQ(slotMap.getSlot(21), 2);
	EXPECT_EQ(slotMap.getSlot(25), 2);
	EXPECT_EQ(slotMap.getSlot(30), 2);
}

TEST(PrimitiveMetadataSlotMapTest, OutOfOrderAddition)
{
	// TSortedVector should keep them sorted regardless of addition order
	PrimitiveMetadataSlotMap slotMap;
	slotMap.addSlot(2, 30);
	slotMap.addSlot(0, 10);
	slotMap.addSlot(1, 20);

	EXPECT_EQ(slotMap.getSlot(0), 0);
	EXPECT_EQ(slotMap.getSlot(10), 0);
	EXPECT_EQ(slotMap.getSlot(11), 1);
	EXPECT_EQ(slotMap.getSlot(20), 1);
	EXPECT_EQ(slotMap.getSlot(21), 2);
	EXPECT_EQ(slotMap.getSlot(30), 2);
}

TEST(PrimitiveMetadataSlotMapTest, CustomSlotMapping)
{
	PrimitiveMetadataSlotMap slotMap;
	slotMap.addSlot(7, 100);
	slotMap.addSlot(3, 200);

	EXPECT_EQ(slotMap.getSlot(0), 7);
	EXPECT_EQ(slotMap.getSlot(100), 7);
	EXPECT_EQ(slotMap.getSlot(101), 3);
	EXPECT_EQ(slotMap.getSlot(200), 3);
}
