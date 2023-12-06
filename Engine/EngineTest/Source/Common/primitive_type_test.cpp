#include <Common/primitive_type.h>

#include <gtest/gtest.h>

#include <climits>

TEST(PrimitiveTypeTest, HasExpectedSize)
{
	EXPECT_EQ(CHAR_BIT * sizeof(ph::int8),    8);
	EXPECT_EQ(CHAR_BIT * sizeof(ph::uint8),   8);
	EXPECT_EQ(CHAR_BIT * sizeof(ph::int32),   32);
	EXPECT_EQ(CHAR_BIT * sizeof(ph::uint32),  32);
	EXPECT_EQ(CHAR_BIT * sizeof(ph::float32), 32);
	EXPECT_EQ(CHAR_BIT * sizeof(ph::int64),   64);
	EXPECT_EQ(CHAR_BIT * sizeof(ph::uint64),  64);
	EXPECT_EQ(CHAR_BIT * sizeof(ph::float64), 64);
}