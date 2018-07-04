#include <Utility/TFixedIntegerMap.h>

#include <gtest/gtest.h>

#include <string>

using namespace ph;

TEST(TFixedIntegerMapTest, GetMappedValues)
{
	TFixedIntegerMap<
		TFixedIntegerMapEntry<3, float>,
		TFixedIntegerMapEntry<2, int>,
		TFixedIntegerMapEntry<1, std::string>
	> intToAnyMap;

	intToAnyMap.get<3>() = 33.0f;
	intToAnyMap.get<2>() = 10000;
	intToAnyMap.get<1>() = "hi";

	EXPECT_EQ(intToAnyMap.get<3>(), 33.0f);
	EXPECT_EQ(intToAnyMap.get<2>(), 10000);
	EXPECT_EQ(intToAnyMap.get<1>(), "hi");
}

TEST(TFixedIntegerMapTest, GetIndexOfEntry)
{
	TFixedIntegerMap<
		TFixedIntegerMapEntry<100, int>,
		TFixedIntegerMapEntry<200, int>,
		TFixedIntegerMapEntry<300, int>
	> intToIntMap;

	intToIntMap.get<100>() = 0;
	intToIntMap.get<200>() = 0;
	intToIntMap.get<300>() = 0;

	EXPECT_EQ(intToIntMap.getEntryIndex<100>(), 0);
	EXPECT_EQ(intToIntMap.getEntryIndex<200>(), 1);
	EXPECT_EQ(intToIntMap.getEntryIndex<300>(), 2);
}