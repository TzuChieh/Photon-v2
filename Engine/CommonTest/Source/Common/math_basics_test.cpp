#include <Common/math_basics.h>

#include <gtest/gtest.h>

#include <limits>
#include <cstdint>

using namespace ph;
using namespace ph::math;

TEST(MathBasicsTest, CheckIsPowerOf2)
{
	EXPECT_FALSE(is_power_of_2(0));
	EXPECT_TRUE (is_power_of_2(1));
	EXPECT_TRUE (is_power_of_2(2));
	EXPECT_FALSE(is_power_of_2(3));
	EXPECT_TRUE (is_power_of_2(1 << 17));
	EXPECT_FALSE(is_power_of_2((1 << 17) + 1));
	EXPECT_FALSE(is_power_of_2(-1));
	EXPECT_FALSE(is_power_of_2(std::numeric_limits<int>::max()));
	EXPECT_FALSE(is_power_of_2(std::numeric_limits<int>::min()));
	EXPECT_FALSE(is_power_of_2(std::numeric_limits<unsigned int>::max()));
	EXPECT_FALSE(is_power_of_2(0ULL));
	EXPECT_TRUE (is_power_of_2(1ULL << 50));
	EXPECT_FALSE(is_power_of_2((1ULL << 50) + 1));
	EXPECT_FALSE(is_power_of_2(std::numeric_limits<unsigned long long>::max()));
}

TEST(MathBasicsTest, CeiledPositiveIntegerDivision)
{
	EXPECT_EQ(ceil_div(3, 3),   1);
	EXPECT_EQ(ceil_div(5, 3),   2);
	EXPECT_EQ(ceil_div(5, 2),   3);
	EXPECT_EQ(ceil_div(0, 3),   0);
	EXPECT_EQ(ceil_div(12, 11), 2);
	EXPECT_EQ(ceil_div(10, 11), 1);

	const std::uint32_t x1 = std::numeric_limits<std::uint32_t>::max() / 2;
	const std::uint32_t x2 = x1;
	EXPECT_EQ(ceil_div(x1, x2), 1);
	EXPECT_EQ(ceil_div(x1 + 1, x2), 2);
	EXPECT_EQ(ceil_div(x1 - 1, x2), 1);
	EXPECT_EQ(ceil_div(x1, x2 - 1), 2);
}

TEST(MathBasicsTest, FindNextIntegerMultiple)
{
	EXPECT_EQ(next_multiple(4, 5), 5);
	EXPECT_EQ(next_multiple(4, 4), 4);
	EXPECT_EQ(next_multiple(4, 3), 6);
	EXPECT_EQ(next_multiple(9, 8), 16);
	EXPECT_EQ(next_multiple(10, 8), 16);
	EXPECT_EQ(next_multiple(11, 8), 16);
	EXPECT_EQ(next_multiple(16, 8), 16);
	EXPECT_EQ(next_multiple(17, 8), 24);
	EXPECT_EQ(next_multiple(0, 10), 0);
	EXPECT_EQ(next_multiple(0, 123), 0);
	EXPECT_EQ(next_multiple(0, 9999), 0);

	const std::uint32_t x1 = std::numeric_limits<std::uint32_t>::max() / 2;
	const std::uint32_t x2 = x1;
	EXPECT_EQ(next_multiple(x1, x2), x2);
	EXPECT_EQ(next_multiple(x1 + 1, x2), x2 * 2);
	EXPECT_EQ(next_multiple(x1 - 1, x2), x2);
	EXPECT_EQ(next_multiple(x1, x2 - 1), (x2 - 1) * 2);
}

TEST(MathBasicsTest, FindNextIntegerPowerOf2Multiple)
{
	EXPECT_EQ(next_power_of_2_multiple(1, 1), 1);
	EXPECT_EQ(next_power_of_2_multiple(2, 1), 2);
	EXPECT_EQ(next_power_of_2_multiple(3, 1), 3);
	EXPECT_EQ(next_power_of_2_multiple(4, 1), 4);

	EXPECT_EQ(next_power_of_2_multiple(1, 2), 2);
	EXPECT_EQ(next_power_of_2_multiple(2, 2), 2);
	EXPECT_EQ(next_power_of_2_multiple(3, 2), 4);
	EXPECT_EQ(next_power_of_2_multiple(4, 2), 4);

	EXPECT_EQ(next_power_of_2_multiple(1, 32), 32);
	EXPECT_EQ(next_power_of_2_multiple(2, 32), 32);
	EXPECT_EQ(next_power_of_2_multiple(32, 32), 32);
	EXPECT_EQ(next_power_of_2_multiple(64, 32), 64);
	EXPECT_EQ(next_power_of_2_multiple(65, 32), 32 * 3);

	const auto x = static_cast<std::uint32_t>(0b01000000'00000000'00000000'00000000);
	EXPECT_EQ(next_power_of_2_multiple(x, x), x);
	EXPECT_EQ(next_power_of_2_multiple(x + 1, x), x * 2);
	EXPECT_EQ(next_power_of_2_multiple(x - 1, x), x);
	EXPECT_EQ(next_power_of_2_multiple(x + 12345, x), x * 2);
	EXPECT_EQ(next_power_of_2_multiple(x + x, x), x * 2);
	EXPECT_EQ(next_power_of_2_multiple(x + x - 1, x), x * 2);
}
