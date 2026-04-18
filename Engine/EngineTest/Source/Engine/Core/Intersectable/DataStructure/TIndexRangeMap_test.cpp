#include <Engine/Core/Intersection/DataStructure/TIndexRangeMap.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(TIndexRangeMapTest, Behavior)
{
	TIndexRangeMap<uint64, uint32> slotMap(3);
	slotMap.setRangeMap(0, 10, 0);
	slotMap.setRangeMap(1, 20, 1);
	slotMap.setRangeMap(2, 30, 2);

	EXPECT_EQ(slotMap.get(0), 0);
	EXPECT_EQ(slotMap.get(5), 0);
	EXPECT_EQ(slotMap.get(10), 0);

	EXPECT_EQ(slotMap.get(11), 1);
	EXPECT_EQ(slotMap.get(15), 1);
	EXPECT_EQ(slotMap.get(20), 1);

	EXPECT_EQ(slotMap.get(21), 2);
	EXPECT_EQ(slotMap.get(25), 2);
	EXPECT_EQ(slotMap.get(30), 2);
}

TEST(TIndexRangeMapTest, CustomSlotMapping)
{
	TIndexRangeMap<uint64, uint32> slotMap(2);
	slotMap.setRangeMap(0, 100, 7);
	slotMap.setRangeMap(1, 200, 3);

	EXPECT_EQ(slotMap.get(0), 7);
	EXPECT_EQ(slotMap.get(100), 7);
	EXPECT_EQ(slotMap.get(101), 3);
	EXPECT_EQ(slotMap.get(200), 3);
}
