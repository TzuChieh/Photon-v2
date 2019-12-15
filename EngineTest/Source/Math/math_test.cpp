#include <Math/math.h>

#include <gtest/gtest.h>

#include <limits>
#include <cstdint>

using namespace ph;
using namespace ph::math;

TEST(MathTest, AngleUnitConversion)
{
	EXPECT_FLOAT_EQ(to_degrees(3.14159265359f), 180.0f);
	EXPECT_FLOAT_EQ(to_radians(180.0f), 3.14159265359f);
}

TEST(MathTest, NumberSignExtraction)
{
	EXPECT_TRUE(sign(-3) == -1);
	EXPECT_TRUE(sign( 0) ==  0);
	EXPECT_TRUE(sign( 2) ==  1);

	EXPECT_TRUE(sign(-4.0f) == -1);
	EXPECT_TRUE(sign( 0.0f) ==  0);
	EXPECT_TRUE(sign( 7.0f) ==  1);

	EXPECT_TRUE(sign(0ULL) == 0);
	EXPECT_TRUE(sign(std::numeric_limits<unsigned long long>::max()) == 1);
}

TEST(MathTest, NumberClamping)
{
	EXPECT_TRUE(clamp(-1, 0, 4) == 0);
	EXPECT_TRUE(clamp( 2, 0, 4) == 2);
	EXPECT_TRUE(clamp( 7, 0, 4) == 4);
	EXPECT_TRUE(clamp( 7, 7, 8) == 7);
	EXPECT_TRUE(clamp( 8, 7, 8) == 8);

	EXPECT_TRUE(clamp(-3.0f, 1.0f, 5.0f) == 1.0f);
	EXPECT_TRUE(clamp( 2.5f, 1.0f, 5.0f) == 2.5f);
	EXPECT_TRUE(clamp( 5.1f, 1.0f, 5.0f) == 5.0f);
	EXPECT_TRUE(clamp( 5.1f, 5.1f, 6.1f) == 5.1f);
	EXPECT_TRUE(clamp( 6.1f, 5.1f, 6.1f) == 6.1f);

	if constexpr(std::numeric_limits<float>::has_quiet_NaN)
	{
		EXPECT_TRUE(clamp(std::numeric_limits<float>::quiet_NaN(), 1.0f, 5.0f) == 1.0f);
	}

	if constexpr(std::numeric_limits<double>::has_quiet_NaN)
	{
		EXPECT_TRUE(clamp(std::numeric_limits<double>::quiet_NaN(), 1.0, 5.0) == 1.0);
	}
}

TEST(MathTest, EvaluateNextPowerOf2)
{
	EXPECT_EQ(next_power_of_2(1), 1);
	EXPECT_EQ(next_power_of_2(2), 2);
	EXPECT_EQ(next_power_of_2(3), 4);
	EXPECT_EQ(next_power_of_2(4), 4);
	EXPECT_EQ(next_power_of_2(5), 8);
	EXPECT_EQ(next_power_of_2(6), 8);
	EXPECT_EQ(next_power_of_2(7), 8);
	EXPECT_EQ(next_power_of_2(8), 8);

	// TODO: test by calculating using log functions

	EXPECT_EQ(next_power_of_2(1023),  1024);
	EXPECT_EQ(next_power_of_2(32700), 32768);

	// special case (this behavior is part of spec.)
	EXPECT_EQ(next_power_of_2(0), 0);
}

TEST(MathTest, CheckIsPowerOf2)
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

TEST(MathTest, CalculateIntegerBase2Logarithm)
{
	EXPECT_EQ(log2_floor(1), 0);
	EXPECT_EQ(log2_floor(2), 1);
	EXPECT_EQ(log2_floor(3), 1);
	EXPECT_EQ(log2_floor(4), 2);
	EXPECT_EQ(log2_floor(5), 2);
	EXPECT_EQ(log2_floor(6), 2);
	EXPECT_EQ(log2_floor(7), 2);
	EXPECT_EQ(log2_floor(8), 3);
	EXPECT_EQ(log2_floor(1024), 10);
	EXPECT_EQ(log2_floor(1 << 20), 20);

	EXPECT_EQ(log2_floor(std::numeric_limits<ph::int8>::max()),   6);
	EXPECT_EQ(log2_floor(std::numeric_limits<ph::uint8>::max()),  7);
	EXPECT_EQ(log2_floor(std::numeric_limits<ph::int16>::max()),  14);
	EXPECT_EQ(log2_floor(std::numeric_limits<ph::uint16>::max()), 15);
	EXPECT_EQ(log2_floor(std::numeric_limits<ph::int32>::max()),  30);
	EXPECT_EQ(log2_floor(std::numeric_limits<ph::uint32>::max()), 31);
	EXPECT_EQ(log2_floor(std::numeric_limits<ph::int64>::max()),  62);
	EXPECT_EQ(log2_floor(std::numeric_limits<ph::uint64>::max()), 63);
}

TEST(MathTest, RetrieveFractionalPartOfANumber)
{
	EXPECT_FLOAT_EQ(fractional_part( 0.00f),  0.00f);
	EXPECT_FLOAT_EQ(fractional_part( 0.22f),  0.22f);
	EXPECT_FLOAT_EQ(fractional_part( 2.33f),  0.33f);
	EXPECT_FLOAT_EQ(fractional_part(-2.44f), -0.44f);

	EXPECT_DOUBLE_EQ(fractional_part( 0.00), 0.00);
	EXPECT_DOUBLE_EQ(fractional_part( 0.44), 0.44);
	EXPECT_DOUBLE_EQ(fractional_part( 3.55), 0.55);
	EXPECT_DOUBLE_EQ(fractional_part(-3.66),-0.66);
}

TEST(MathTest, ConstructsMatrix)
{
	auto matrix = matrix2x2(
		1, 2, 
		3, 4
	);

	EXPECT_EQ(matrix[0][0], 1);
	EXPECT_EQ(matrix[0][1], 2);
	EXPECT_EQ(matrix[1][0], 3);
	EXPECT_EQ(matrix[1][1], 4);
}

TEST(MathTest, SolvesLinearSystem)
{
	// trial 1

	std::array<float, 2> x1;

	const auto A1 = matrix2x2(
		1.0f, 0.0f, 
		0.0f, 1.0f
	);

	const bool isX1Solved = solve_linear_system_2x2(A1, {{1.0f, 1.0f}}, &x1);
	ASSERT_TRUE(isX1Solved);

	EXPECT_FLOAT_EQ(x1[0], 1.0f);
	EXPECT_FLOAT_EQ(x1[1], 1.0f);

	// trial 2

	std::array<float, 2> x2;

	const auto A2 = matrix2x2(
		2.0f, 1.0f,
		1.0f, 2.0f
	);

	const bool isX2Solved = solve_linear_system_2x2(A2, {{3.0f, 3.0f}}, &x2);
	ASSERT_TRUE(isX2Solved);

	EXPECT_FLOAT_EQ(x2[0], 1.0f);
	EXPECT_FLOAT_EQ(x2[1], 1.0f);

	// trial 3

	std::array<float, 2> x3;

	const auto A3 = matrix2x2(
		-7.0f, -7.0f,
		-7.0f, -7.0f
	);

	const bool isX3Solved = solve_linear_system_2x2(A3, {{14.0f, 14.0f}}, &x3);
	ASSERT_FALSE(isX3Solved);
}

TEST(MathTest, IntegerWrapping)
{
	using namespace ph;

	EXPECT_EQ(math::wrap(-5, -1, 2), -1);
	EXPECT_EQ(math::wrap(-4, -1, 2),  0);
	EXPECT_EQ(math::wrap(-3, -1, 2),  1);
	EXPECT_EQ(math::wrap(-2, -1, 2),  2);
	EXPECT_EQ(math::wrap(-1, -1, 2), -1);
	EXPECT_EQ(math::wrap( 0, -1, 2),  0);
	EXPECT_EQ(math::wrap( 1, -1, 2),  1);
	EXPECT_EQ(math::wrap( 2, -1, 2),  2);
	EXPECT_EQ(math::wrap( 3, -1, 2), -1);

	const int maxInt = std::numeric_limits<int>::max();
	EXPECT_EQ(math::wrap(maxInt - 11, maxInt - 10, maxInt - 8), maxInt - 8);
	EXPECT_EQ(math::wrap(maxInt - 7,  maxInt - 10, maxInt - 8), maxInt - 10);

	const int minInt = std::numeric_limits<int>::min();
	EXPECT_EQ(math::wrap(minInt + 2, minInt + 3, minInt + 7), minInt + 7);
	EXPECT_EQ(math::wrap(minInt + 8, minInt + 3, minInt + 7), minInt + 3);

	EXPECT_EQ(math::wrap(0U, 1U, 2U), 2U);
	EXPECT_EQ(math::wrap(1U, 1U, 2U), 1U);
	EXPECT_EQ(math::wrap(2U, 1U, 2U), 2U);
	EXPECT_EQ(math::wrap(3U, 1U, 2U), 1U);
	EXPECT_EQ(math::wrap(4U, 1U, 2U), 2U);
	EXPECT_EQ(math::wrap(5U, 1U, 2U), 1U);

	const unsigned int maxUint = std::numeric_limits<unsigned int>::max();
	EXPECT_EQ(math::wrap(maxUint - 11, maxUint - 10, maxUint - 8), maxUint - 8);
	EXPECT_EQ(math::wrap(maxUint - 7,  maxUint - 10, maxUint - 8), maxUint - 10);
}

TEST(MathTest, EvenlyDividedRanges)
{
	const std::size_t size1 = 10;
	const std::size_t numDivisions1 = 10;
	for(std::size_t i = 0; i < numDivisions1; ++i)
	{
		const auto range = ith_evenly_divided_range(i, size1, numDivisions1);
		EXPECT_EQ(range.first + 1, range.second);
	}

	const std::size_t size2 = 98765;
	const std::size_t numDivisions2 = 77;

	std::size_t summedSize2 = 0;
	for(std::size_t i = 0; i < numDivisions2; ++i)
	{
		const auto range     = ith_evenly_divided_range(i, size2, numDivisions2);
		const auto rangeSize = range.second - range.first;
		summedSize2 += rangeSize;

		// 98765 / 1282 = 1282.6623...
		EXPECT_TRUE(rangeSize == 1282 || rangeSize == 1283);
	}
	EXPECT_EQ(summedSize2, size2);

	const std::size_t size3 = 0;
	const std::size_t numDivisions3 = 9;
	for(std::size_t i = 0; i < numDivisions3; ++i)
	{
		const auto range = ith_evenly_divided_range(i, size3, numDivisions3);
		EXPECT_EQ(range.first, range.second);
	}
}

TEST(MathTest, FastReciprocalSqrt)
{
	for(double x = 0.000001; x < 1000000.0; x = x < 1.0 ? x + 0.0000017 : x + 17.0)
	{
		const float  fastResult = fast_rcp_sqrt(static_cast<float>(x));
		const double goodResult = std::sqrt(1.0 / x);

		const double relativeError = std::abs(fastResult - goodResult) / goodResult;

		// Current implementation should have < 0.175125% max. relative error,
		// slightly increase the threshold to account for numerical error.
		EXPECT_LT(relativeError * 100.0, 0.175130);
	}
}

TEST(MathTest, FastSqrt)
{
	for(double x = 0.000001; x < 1000000.0; x = x < 1.0 ? x + 0.0000017 : x + 17.0)
	{
		const float  fastResult = fast_sqrt(static_cast<float>(x));
		const double goodResult = std::sqrt(x);

		const double relativeError = std::abs(fastResult - goodResult) / goodResult;

		// accept at most 1% max. relative error
		EXPECT_LT(relativeError * 100.0, 1.0);
	}
}

TEST(MathTest, CeiledPositiveIntegerDivision)
{
	EXPECT_EQ(ceil_div_positive(3, 3),   1);
	EXPECT_EQ(ceil_div_positive(5, 3),   2);
	EXPECT_EQ(ceil_div_positive(5, 2),   3);
	EXPECT_EQ(ceil_div_positive(0, 3),   0);
	EXPECT_EQ(ceil_div_positive(12, 11), 2);
	EXPECT_EQ(ceil_div_positive(10, 11), 1);

	const std::uint32_t x1 = std::numeric_limits<std::uint32_t>::max() / 2;
	const std::uint32_t x2 = x1;
	EXPECT_EQ(ceil_div_positive(x1, x2), 1);
	EXPECT_EQ(ceil_div_positive(x1 + 1, x2), 2);
	EXPECT_EQ(ceil_div_positive(x1 - 1, x2), 1);
	EXPECT_EQ(ceil_div_positive(x1, x2 - 1), 2);
}

TEST(MathTest, BitReversing)
{
	{
		using Bits = std::uint8_t;

		EXPECT_EQ(reverse_bits(Bits(0b00000000)), 
		                       Bits(0b00000000));

		EXPECT_EQ(reverse_bits(Bits(0b00000001)), 
		                       Bits(0b10000000));

		EXPECT_EQ(reverse_bits(Bits(0b00100000)), 
		                       Bits(0b00000100));
	}

	{
		using Bits = std::uint16_t;

		EXPECT_EQ(reverse_bits(Bits(0b00000000'00000000)), 
		                       Bits(0b00000000'00000000));

		EXPECT_EQ(reverse_bits(Bits(0b10000000'00000000)), 
		                       Bits(0b00000000'00000001));

		EXPECT_EQ(reverse_bits(Bits(0b00000010'00000000)), 
		                       Bits(0b00000000'01000000));
	}

	{
		using Bits = std::uint32_t;

		EXPECT_EQ(reverse_bits(Bits(0b00000000'00000000'00000000'00000000)), 
		                       Bits(0b00000000'00000000'00000000'00000000));

		EXPECT_EQ(reverse_bits(Bits(0b10000000'00000000'00000000'00000000)), 
		                       Bits(0b00000000'00000000'00000000'00000001));

		EXPECT_EQ(reverse_bits(Bits(0b00000000'00000000'00000000'00000001)), 
		                       Bits(0b10000000'00000000'00000000'00000000));

		EXPECT_EQ(reverse_bits(Bits(0b00000000'00000000'00000000'10101010)), 
		                       Bits(0b01010101'00000000'00000000'00000000));

		EXPECT_EQ(reverse_bits(Bits(0b00000011'10000001'00000000'00000000)), 
		                       Bits(0b00000000'00000000'10000001'11000000));

		EXPECT_EQ(reverse_bits(Bits(0b01100011'11101001'00000000'00000000)), 
		                       Bits(0b00000000'00000000'10010111'11000110));

		EXPECT_EQ(reverse_bits(Bits(0b11111111'11111111'11111111'11111111)), 
		                       Bits(0b11111111'11111111'11111111'11111111));
	}

	{
		using Bits = std::uint64_t;

		EXPECT_EQ(reverse_bits(Bits(0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000)), 
		                       Bits(0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000));

		EXPECT_EQ(reverse_bits(Bits(0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000001)), 
		                       Bits(0b10000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000));

		EXPECT_EQ(reverse_bits(Bits(0b10000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000)), 
		                       Bits(0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000001));

		EXPECT_EQ(reverse_bits(Bits(0b00000000'00000111'00000001'00000000'00000000'00000000'00000000'00000001)), 
		                       Bits(0b10000000'00000000'00000000'00000000'00000000'10000000'11100000'00000000));

		EXPECT_EQ(reverse_bits(Bits(0b00000000'10101010'00000000'11111111'00000000'00000000'00000000'00000000)), 
		                       Bits(0b00000000'00000000'00000000'00000000'11111111'00000000'01010101'00000000));
	}
}
