#include <Utility/TFixedIntegerMap.h>
#include <Utility/TFixedEnumMap.h>

#include <gtest/gtest.h>

#include <string>

using namespace ph;

TEST(TFixedIntegerMapTest, GetMappedValues)
{
	TFixedIntegerMap<
		TFixedIntegerMapEntry<3, float>,
		TFixedIntegerMapEntry<2, int>,
		TFixedIntegerMapEntry<1, std::string>
	> intToAnyMap1;

	intToAnyMap1.get<3>() = 33.0f;
	intToAnyMap1.get<2>() = 10000;
	intToAnyMap1.get<1>() = "hi";

	EXPECT_EQ(intToAnyMap1.get<3>(), 33.0f);
	EXPECT_EQ(intToAnyMap1.get<2>(), 10000);
	EXPECT_EQ(intToAnyMap1.get<1>(), "hi");
}

TEST(TFixedIntegerMapTest, GetIndexOfEntry)
{
	TFixedIntegerMap<
		TFixedIntegerMapEntry<100, int>,
		TFixedIntegerMapEntry<200, int>,
		TFixedIntegerMapEntry<300, int>
	> intToIntMap;

	EXPECT_EQ(intToIntMap.getEntryIndex<100>(), 0);
	EXPECT_EQ(intToIntMap.getEntryIndex<200>(), 1);
	EXPECT_EQ(intToIntMap.getEntryIndex<300>(), 2);
}

TEST(TFixedEnumMapTest, GetMappedValues)
{
	enum class TestEnum
	{
		A, B, C
	};

	TFixedEnumMap<
		TFixedEnumMapEntry<TestEnum::A, float>,
		TFixedEnumMapEntry<TestEnum::B, int>,
		TFixedEnumMapEntry<TestEnum::C, std::string>
	> enumToAnyMap1;

	enumToAnyMap1.get<TestEnum::A>() = 1.0f;
	enumToAnyMap1.get<TestEnum::B>() = 2;
	enumToAnyMap1.get<TestEnum::C>() = "yo";

	EXPECT_EQ(enumToAnyMap1.get<TestEnum::A>(), 1.0f);
	EXPECT_EQ(enumToAnyMap1.get<TestEnum::B>(), 2);
	EXPECT_EQ(enumToAnyMap1.get<TestEnum::C>(), "yo");
}

TEST(TFixedEnumMapTest, GetIndexOfEntry)
{
	/*enum class TestEnum
	{
		A, B, C
	};

	TFixedEnumMap<
		TFixedEnumMapEntry<TestEnum::A, int>,
		TFixedEnumMapEntry<TestEnum::B, int>,
		TFixedEnumMapEntry<TestEnum::C, int>
	> enumToIntMap;

	EXPECT_EQ(enumToIntMap.getEntryIndex<TestEnum::A>(), 0);
	EXPECT_EQ(enumToIntMap.getEntryIndex<TestEnum::B>(), 1);
	EXPECT_EQ(enumToIntMap.getEntryIndex<TestEnum::C>(), 2);*/
}