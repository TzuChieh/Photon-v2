#include <Math/math.h>

#include <gtest/gtest.h>

#include <limits>
#include <cstdint>
#include <cmath>

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
	// Ordinary clamping
	{
		EXPECT_EQ(clamp(-1, 0, 4), 0);
		EXPECT_EQ(clamp( 2, 0, 4), 2);
		EXPECT_EQ(clamp( 7, 0, 4), 4);
		EXPECT_EQ(clamp( 7, 7, 8), 7);
		EXPECT_EQ(clamp( 8, 7, 8), 8);

		EXPECT_EQ(clamp(-3.0f, 1.0f, 5.0f), 1.0f);
		EXPECT_EQ(clamp( 2.5f, 1.0f, 5.0f), 2.5f);
		EXPECT_EQ(clamp( 5.1f, 1.0f, 5.0f), 5.0f);
		EXPECT_EQ(clamp( 5.1f, 5.1f, 6.1f), 5.1f);
		EXPECT_EQ(clamp( 6.1f, 5.1f, 6.1f), 6.1f);

		EXPECT_EQ(clamp(std::numeric_limits<float>::lowest(), -3000.0f, 123456.0f), -3000.0f);
		EXPECT_EQ(clamp(std::numeric_limits<double>::max(), 1e9, 1e32), 1e32);
	}

	// Safe clamping: NaN float
	if constexpr(std::numeric_limits<float>::has_quiet_NaN)
	{
		constexpr auto nan = std::numeric_limits<float>::quiet_NaN();

		EXPECT_EQ(safe_clamp(nan, 1.0f, 5.0f), 1.0f);
		EXPECT_EQ(safe_clamp(nan, -3.2f, -0.1f), -3.2f);
	}

	// Safe clamping: NaN double
	if constexpr(std::numeric_limits<double>::has_quiet_NaN)
	{
		constexpr auto nan = std::numeric_limits<double>::quiet_NaN();

		EXPECT_EQ(safe_clamp(nan, 2.0, 5.0), 2.0);
		EXPECT_EQ(safe_clamp(nan, -5.5, -2.2), -5.5);
	}

	// Safe clamping: Inf float
	if constexpr(std::numeric_limits<float>::has_infinity)
	{
		constexpr auto inf = std::numeric_limits<float>::infinity();

		EXPECT_EQ(safe_clamp(inf, 1.0f, 5.0f), 1.0f);
		EXPECT_EQ(safe_clamp(-inf, 1.0f, 5.0f), 1.0f);
		EXPECT_EQ(safe_clamp(inf, -5.0f, -1.0f), -5.0f);
		EXPECT_EQ(safe_clamp(inf, -1.0f, inf), -1.0f);
		EXPECT_EQ(safe_clamp(inf, inf, inf), inf);
		EXPECT_EQ(safe_clamp(-inf, inf, inf), inf);
		EXPECT_EQ(safe_clamp(-inf, -inf, inf), -inf);
	}

	// Safe clamping: Inf double
	if constexpr(std::numeric_limits<double>::has_infinity)
	{
		constexpr auto inf = std::numeric_limits<double>::infinity();

		EXPECT_EQ(safe_clamp(inf, 1.0, 5.0), 1.0);
		EXPECT_EQ(safe_clamp(-inf, 1.0, 5.0), 1.0);
		EXPECT_EQ(safe_clamp(inf, -5.0, -1.0), -5.0);
		EXPECT_EQ(safe_clamp(inf, -1.0, inf), -1.0);
		EXPECT_EQ(safe_clamp(inf, inf, inf), inf);
		EXPECT_EQ(safe_clamp(-inf, inf, inf), inf);
		EXPECT_EQ(safe_clamp(-inf, -inf, inf), -inf);
	}
}

TEST(MathTest, NumberReciprocal)
{
	// `int` always result in 0 except for +-1
	{
		for(int n = -1000; n <= 1000; ++n)
		{
			if(std::abs(n) != 1)
			{
				EXPECT_EQ(safe_rcp(n), 0);
			}
			else
			{
				EXPECT_EQ(safe_rcp(n), n);
			}
		}
	}
	
	// `float`
	{
		EXPECT_EQ(safe_rcp(0.0f), 0.0f);
		EXPECT_EQ(safe_rcp(1.0f), 1.0f);
		EXPECT_EQ(safe_rcp(-1.0f), -1.0f);
		EXPECT_GT(safe_rcp(std::numeric_limits<float>::max()), 0.0f);
		EXPECT_LT(safe_rcp(std::numeric_limits<float>::lowest()), 0.0f);
		EXPECT_GT(safe_rcp(std::numeric_limits<float>::min()), 0.0f);

		if constexpr(std::numeric_limits<float>::has_infinity)
		{
			EXPECT_EQ(safe_rcp(std::numeric_limits<float>::infinity()), 0.0f);
		}

		if constexpr(std::numeric_limits<float>::has_quiet_NaN)
		{
			EXPECT_EQ(safe_rcp(std::numeric_limits<float>::quiet_NaN()), 0.0f);
		}

		if constexpr(std::numeric_limits<float>::has_signaling_NaN)
		{
			EXPECT_EQ(safe_rcp(std::numeric_limits<float>::signaling_NaN()), 0.0f);
		}
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

	// Special case (this behavior is part of spec.)
	EXPECT_EQ(next_power_of_2(0), 0);
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

	constexpr int maxInt = std::numeric_limits<int>::max();
	EXPECT_EQ(math::wrap(maxInt - 11, maxInt - 10, maxInt - 8), maxInt - 8);
	EXPECT_EQ(math::wrap(maxInt - 7,  maxInt - 10, maxInt - 8), maxInt - 10);

	constexpr int minInt = std::numeric_limits<int>::min();
	EXPECT_EQ(math::wrap(minInt + 2, minInt + 3, minInt + 7), minInt + 7);
	EXPECT_EQ(math::wrap(minInt + 8, minInt + 3, minInt + 7), minInt + 3);

	EXPECT_EQ(math::wrap(0U, 1U, 2U), 2U);
	EXPECT_EQ(math::wrap(1U, 1U, 2U), 1U);
	EXPECT_EQ(math::wrap(2U, 1U, 2U), 2U);
	EXPECT_EQ(math::wrap(3U, 1U, 2U), 1U);
	EXPECT_EQ(math::wrap(4U, 1U, 2U), 2U);
	EXPECT_EQ(math::wrap(5U, 1U, 2U), 1U);

	constexpr unsigned int maxUint = std::numeric_limits<unsigned int>::max();
	EXPECT_EQ(math::wrap(maxUint - 11, maxUint - 10, maxUint - 8), maxUint - 8);
	EXPECT_EQ(math::wrap(maxUint - 7,  maxUint - 10, maxUint - 8), maxUint - 10);

	// Range size = 1 ([x, x], singed)
	{
		for(int x = -1000; x <= 1000; ++x)
		{
			for(int offset = -10; offset <= 10; ++offset)
			{
				// No matter the offset amount, always wrap to same value as range = 1
				EXPECT_EQ(math::wrap(x + offset, x, x), x);
			}
		}
	}

	// Range size = 1 ([x, x], unsigned)
	{
		for(unsigned int x = 0; x <= 1000; ++x)
		{
			for(unsigned int offset = 0; offset <= 100; ++offset)
			{
				// No matter the offset amount, always wrap to same value as range = 1
				EXPECT_EQ(math::wrap(x + offset, x, x), x);
			}
		}
	}
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

		// Accept at most 1% max. relative error
		EXPECT_LT(relativeError * 100.0, 1.0);
	}
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

		EXPECT_EQ(reverse_bits(Bits(0b00000000'10101010'00000000'11111111'00000000'00000011'10000000'00000000)), 
		                       Bits(0b00000000'00000001'11000000'00000000'11111111'00000000'01010101'00000000));
	}
}

TEST(MathTest, HalfFloatConversions)
{
	// Test values and bit patterns can be found at:
	// https://en.wikipedia.org/wiki/Half-precision_floating-point_format

	EXPECT_FLOAT_EQ(fp16_bits_to_fp32(0b0'00000'0000000000), 0.0f);
	EXPECT_FLOAT_EQ(fp32_to_fp16_bits(0.0f), 0b0'00000'0000000000);

	EXPECT_FLOAT_EQ(fp16_bits_to_fp32(0b0'01111'0000000000), 1.0f);
	EXPECT_FLOAT_EQ(fp32_to_fp16_bits(1.0f), 0b0'01111'0000000000);

	EXPECT_FLOAT_EQ(fp16_bits_to_fp32(0b1'00000'0000000000), -0.0f);
	EXPECT_FLOAT_EQ(fp32_to_fp16_bits(-0.0f), 0b1'00000'0000000000);

	EXPECT_FLOAT_EQ(fp16_bits_to_fp32(0b1'10000'0000000000), -2.0f);
	EXPECT_FLOAT_EQ(fp32_to_fp16_bits(-2.0f), 0b1'10000'0000000000);

	// Smallest number larger than one 
	EXPECT_FLOAT_EQ(fp16_bits_to_fp32(0b0'01111'0000000001), 1.00097656f);

	// Largest normal number
	EXPECT_FLOAT_EQ(fp16_bits_to_fp32(0b0'11110'1111111111), 65504.0f);

	// Nearest value to 1/3
	EXPECT_FLOAT_EQ(fp16_bits_to_fp32(0b0'01101'0101010101), 0.33325195f);
}

TEST(MathTest, NormalizeInteger)
{
	EXPECT_FLOAT_EQ(normalize_integer<float>(0), 0.0f);
	EXPECT_FLOAT_EQ(normalize_integer<float>(std::numeric_limits<int>::max()), 1.0f);
	EXPECT_FLOAT_EQ(normalize_integer<float>(std::numeric_limits<int>::min()), -1.0f);
	EXPECT_FLOAT_EQ(normalize_integer<float>(std::numeric_limits<int>::max() / 2), 0.5f);
	EXPECT_FLOAT_EQ(normalize_integer<float>(std::numeric_limits<int>::min() / 2), -0.5f);

	EXPECT_FLOAT_EQ(normalize_integer<float>(0U), 0.0f);
	EXPECT_FLOAT_EQ(normalize_integer<float>(std::numeric_limits<unsigned int>::max()), 1.0f);
	EXPECT_FLOAT_EQ(normalize_integer<float>(std::numeric_limits<unsigned int>::min()), 0.0f);
	EXPECT_FLOAT_EQ(normalize_integer<float>(std::numeric_limits<unsigned int>::max() / 2), 0.5f);
}

TEST(MathTest, QuantizeFloat)
{
	EXPECT_EQ(quantize_normalized_float<uint8>(0.0f), 0);
	EXPECT_EQ(quantize_normalized_float<uint8>(1.0f), 255);
	EXPECT_EQ(quantize_normalized_float<uint8>(0.5f + 0.001f), 128);
	EXPECT_EQ(quantize_normalized_float<uint8>(0.5f - 0.001f), 127);

	EXPECT_EQ(quantize_normalized_float<int>(0.0f), 0);
	EXPECT_EQ(quantize_normalized_float<int>(-1.0f), std::numeric_limits<int>::min());
	EXPECT_EQ(quantize_normalized_float<int>(1.0f), std::numeric_limits<int>::max());
}

TEST(MathTest, SetBitsInRange)
{
	// Normal 32-bit
	EXPECT_EQ(set_bits_in_range<uint32>(0b0011'1100, 0, 2), 0b0011'1111);
	EXPECT_EQ(set_bits_in_range<uint32>(0b1000'1110, 4, 7), 0b1111'1110);
	EXPECT_EQ(set_bits_in_range<uint32>(0b0110'0000, 7, 8), 0b1110'0000);
	EXPECT_EQ(set_bits_in_range<uint32>(0b1111'1111, 5, 8), 0b1111'1111);

	// Normal 8-bit
	EXPECT_EQ(set_bits_in_range<uint8>(0b0000'0000, 0, 4), 0b0000'1111);
	EXPECT_EQ(set_bits_in_range<uint8>(0b0000'0000, 4, 8), 0b1111'0000);
	EXPECT_EQ(set_bits_in_range<uint8>(0b1110'0111, 3, 5), 0b1111'1111);
	EXPECT_EQ(set_bits_in_range<uint8>(0b0001'1000, 4, 7), 0b0111'1000);

	// Empty range
	EXPECT_EQ(set_bits_in_range<uint32>(0b1100'1110'1001'1101, 3, 3), 0b1100'1110'1001'1101);
	EXPECT_EQ(set_bits_in_range<uint8>(0b1001'1101, 7, 7), 0b1001'1101);
	EXPECT_EQ(set_bits_in_range<uint8>(0b1011'1011, 8, 8), 0b1011'1011);
}

TEST(MathTest, ClearBitsInRange)
{
	// Normal 32-bit
	EXPECT_EQ(clear_bits_in_range<uint32>(0b1111'1111, 0, 2), 0b1111'1100);
	EXPECT_EQ(clear_bits_in_range<uint32>(0b1111'1111, 4, 7), 0b1000'1111);
	EXPECT_EQ(clear_bits_in_range<uint32>(0b1111'1111, 7, 8), 0b0111'1111);
	EXPECT_EQ(clear_bits_in_range<uint32>(0b0000'0000, 5, 8), 0b0000'0000);

	// Normal 8-bit
	EXPECT_EQ(clear_bits_in_range<uint8>(0b0000'1111, 0, 4), 0b0000'0000);
	EXPECT_EQ(clear_bits_in_range<uint8>(0b1111'0000, 4, 8), 0b0000'0000);
	EXPECT_EQ(clear_bits_in_range<uint8>(0b1111'1111, 3, 5), 0b1110'0111);
	EXPECT_EQ(clear_bits_in_range<uint8>(0b0000'0000, 4, 7), 0b0000'0000);

	// Empty range
	EXPECT_EQ(clear_bits_in_range<uint32>(0b1100'1110'1001'1101, 3, 3), 0b1100'1110'1001'1101);
	EXPECT_EQ(clear_bits_in_range<uint8>(0b1001'1101, 7, 7), 0b1001'1101);
	EXPECT_EQ(clear_bits_in_range<uint8>(0b1011'1011, 8, 8), 0b1011'1011);
}

TEST(MathTest, FlagBit)
{
	// runtime
	{
		EXPECT_EQ(flag_bit<uint8>(0), 0b0000'0001);
		EXPECT_EQ(flag_bit<uint8>(1), 0b0000'0010);
		EXPECT_EQ(flag_bit<uint16>(2), 0b0000'0100);
		EXPECT_EQ(flag_bit<uint16>(3), 0b0000'1000);
		EXPECT_EQ(flag_bit<uint32>(4), 0b0001'0000);
		EXPECT_EQ(flag_bit<uint32>(5), 0b0010'0000);
		EXPECT_EQ(flag_bit<uint32>(6), 0b0100'0000);
		EXPECT_EQ(flag_bit<uint32>(7), 0b1000'0000);

		EXPECT_EQ(flag_bit<uint32>(30), 0b0100'0000'0000'0000'0000'0000'0000'0000);
		EXPECT_EQ(flag_bit<uint32>(31), 0b1000'0000'0000'0000'0000'0000'0000'0000);
	}
	
	// compile-time
	{
		EXPECT_EQ((flag_bit<uint8, 0>()), 0b0000'0001);
		EXPECT_EQ((flag_bit<uint8, 1>()), 0b0000'0010);
		EXPECT_EQ((flag_bit<uint16, 2>()), 0b0000'0100);
		EXPECT_EQ((flag_bit<uint16, 3>()), 0b0000'1000);
		EXPECT_EQ((flag_bit<uint32, 4>()), 0b0001'0000);
		EXPECT_EQ((flag_bit<uint32, 5>()), 0b0010'0000);
		EXPECT_EQ((flag_bit<uint32, 6>()), 0b0100'0000);
		EXPECT_EQ((flag_bit<uint32, 7>()), 0b1000'0000);

		EXPECT_EQ((flag_bit<uint32, 30>()), 0b0100'0000'0000'0000'0000'0000'0000'0000);
		EXPECT_EQ((flag_bit<uint32, 31>()), 0b1000'0000'0000'0000'0000'0000'0000'0000);
	}
}

TEST(MathTest, UInt64MulTo128)
{
	// Multiplying with 0
	{
		{
			uint64 high64, low64;
			uint64_mul(0, 0, high64, low64);
			EXPECT_EQ(high64, 0); EXPECT_EQ(low64, 0);
		}
		{
			uint64 high64, low64;
			uint64_mul(0, 12345, high64, low64);
			EXPECT_EQ(high64, 0); EXPECT_EQ(low64, 0);
		}
		{
			uint64 high64, low64;
			uint64_mul(56789, 0, high64, low64);
			EXPECT_EQ(high64, 0); EXPECT_EQ(low64, 0);
		}
	}

	// Multiplying with 1
	{
		{
			uint64 high64, low64;
			uint64_mul(1, 1, high64, low64);
			EXPECT_EQ(high64, 0); EXPECT_EQ(low64, 1);
		}
		{
			uint64 high64, low64;
			uint64_mul(16, 1, high64, low64);
			EXPECT_EQ(high64, 0); EXPECT_EQ(low64, 16);
		}
		{
			uint64 high64, low64;
			uint64_mul(1, 65536, high64, low64);
			EXPECT_EQ(high64, 0); EXPECT_EQ(low64, 65536);
		}
		{
			uint64 high64, low64;
			uint64_mul(1, std::numeric_limits<uint64>::max(), high64, low64);
			EXPECT_EQ(high64, 0); EXPECT_EQ(low64, std::numeric_limits<uint64>::max());
		}
		{
			uint64 high64, low64;
			uint64_mul(std::numeric_limits<uint64>::max(), 1, high64, low64);
			EXPECT_EQ(high64, 0); EXPECT_EQ(low64, std::numeric_limits<uint64>::max());
		}
	}

	// Multiplying with various large numbers
	{
		{
			uint64 high64, low64;
			uint64_mul(0xFFFFFFFFull, 0xFFFFFFFFull, high64, low64);
			EXPECT_EQ(high64, 0); EXPECT_EQ(low64, 0xFFFFFFFE00000001ull);
		}
		{
			uint64 high64, low64;
			uint64_mul(0xFFFFFFFFFFull, 0xFFFFFFFFFFull, high64, low64); 
			EXPECT_EQ(high64, 0xFFFFull); EXPECT_EQ(low64, 0xFFFFFE0000000001ull);
		}
		{
			uint64 high64, low64;
			uint64_mul(0xABCDABCDABCDull, 0xABCDABCDABCDull, high64, low64); 
			EXPECT_EQ(high64, 0x734C68C1ull); EXPECT_EQ(low64, 0x5E356D12779D8229ull);
		}
		{
			uint64 high64, low64;
			uint64_mul(0xFFFFFFFFFFFFull, 0xAAAABBBBCCCCDDDDull, high64, low64);
			EXPECT_EQ(high64, 0xAAAABBBBCCCCull); EXPECT_EQ(low64, 0x3332444433332223ull);
		}
	}
}
