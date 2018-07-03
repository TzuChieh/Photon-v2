#include <Utility/TFixedIntegerMap.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(TFixedIntegerMapTest, GetMappedValues)
{
	TFixedIntegerMap<
		TFixedIntegerMapEntry<3, float>,
		TFixedIntegerMapEntry<2, float>,
		TFixedIntegerMapEntry<1, float>
	> intToFloatMap;

	intToFloatMap.get<3>() = 33.0f;
	intToFloatMap.get<2>() = 22.0f;
	intToFloatMap.get<1>() = 11.0f;

	EXPECT_FLOAT_EQ(intToFloatMap.get<3>(), 33.0f);
	EXPECT_FLOAT_EQ(intToFloatMap.get<2>(), 22.0f);
	EXPECT_FLOAT_EQ(intToFloatMap.get<1>(), 11.0f);

	// TODO: map classes
}

// TODO: getEntryIndex() test