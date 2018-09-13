#include <Math/math.h>

#include <gtest/gtest.h>

#include <limits>

TEST(MathTest, AngleUnitConversion)
{
	EXPECT_FLOAT_EQ(ph::math::toDegrees(3.14159265359f), 180.0f);
	EXPECT_FLOAT_EQ(ph::math::toRadians(180.0f), 3.14159265359f);
}

TEST(MathTest, NumberSignExtraction)
{
	EXPECT_TRUE(ph::math::sign(-3) == -1);
	EXPECT_TRUE(ph::math::sign( 0) ==  0);
	EXPECT_TRUE(ph::math::sign( 2) ==  1);

	EXPECT_TRUE(ph::math::sign(-4.0f) == -1);
	EXPECT_TRUE(ph::math::sign( 0.0f) ==  0);
	EXPECT_TRUE(ph::math::sign( 7.0f) ==  1);

	EXPECT_TRUE(ph::math::sign(0ULL) == 0);
	EXPECT_TRUE(ph::math::sign(std::numeric_limits<unsigned long long>::max()) == 1);
}

TEST(MathTest, NumberClamping)
{
	EXPECT_TRUE(ph::math::clamp(-1, 0, 4) == 0);
	EXPECT_TRUE(ph::math::clamp( 2, 0, 4) == 2);
	EXPECT_TRUE(ph::math::clamp( 7, 0, 4) == 4);
	EXPECT_TRUE(ph::math::clamp( 7, 7, 8) == 7);
	EXPECT_TRUE(ph::math::clamp( 8, 7, 8) == 8);

	EXPECT_TRUE(ph::math::clamp(-3.0f, 1.0f, 5.0f) == 1.0f);
	EXPECT_TRUE(ph::math::clamp( 2.5f, 1.0f, 5.0f) == 2.5f);
	EXPECT_TRUE(ph::math::clamp( 5.1f, 1.0f, 5.0f) == 5.0f);
	EXPECT_TRUE(ph::math::clamp( 5.1f, 5.1f, 6.1f) == 5.1f);
	EXPECT_TRUE(ph::math::clamp( 6.1f, 5.1f, 6.1f) == 6.1f);

	if constexpr(std::numeric_limits<float>::has_quiet_NaN)
	{
		EXPECT_TRUE(ph::math::clamp(std::numeric_limits<float>::quiet_NaN(), 1.0f, 5.0f) == 1.0f);
	}

	if constexpr(std::numeric_limits<double>::has_quiet_NaN)
	{
		EXPECT_TRUE(ph::math::clamp(std::numeric_limits<double>::quiet_NaN(), 1.0, 5.0) == 1.0);
	}
}

TEST(MathTest, EvaluateNextPowerOf2)
{
	EXPECT_EQ(ph::math::nextPowerOf2(1), 1);
	EXPECT_EQ(ph::math::nextPowerOf2(2), 2);
	EXPECT_EQ(ph::math::nextPowerOf2(3), 4);
	EXPECT_EQ(ph::math::nextPowerOf2(4), 4);
	EXPECT_EQ(ph::math::nextPowerOf2(5), 8);
	EXPECT_EQ(ph::math::nextPowerOf2(6), 8);
	EXPECT_EQ(ph::math::nextPowerOf2(7), 8);
	EXPECT_EQ(ph::math::nextPowerOf2(8), 8);

	// TODO: test by calculating using log functions

	EXPECT_EQ(ph::math::nextPowerOf2(1023),  1024);
	EXPECT_EQ(ph::math::nextPowerOf2(32700), 32768);

	// special case (this behavior is part of spec.)
	EXPECT_EQ(ph::math::nextPowerOf2(0), 0);
}

TEST(MathTest, CheckIsPowerOf2)
{
	EXPECT_FALSE(ph::math::isPowerOf2(0));
	EXPECT_TRUE (ph::math::isPowerOf2(1));
	EXPECT_TRUE (ph::math::isPowerOf2(2));
	EXPECT_FALSE(ph::math::isPowerOf2(3));
	EXPECT_TRUE (ph::math::isPowerOf2(1 << 17));
	EXPECT_FALSE(ph::math::isPowerOf2((1 << 17) + 1));
	EXPECT_FALSE(ph::math::isPowerOf2(-1));
	EXPECT_FALSE(ph::math::isPowerOf2(std::numeric_limits<int>::max()));
	EXPECT_FALSE(ph::math::isPowerOf2(std::numeric_limits<int>::min()));
	EXPECT_FALSE(ph::math::isPowerOf2(std::numeric_limits<unsigned int>::max()));
	EXPECT_FALSE(ph::math::isPowerOf2(0ULL));
	EXPECT_TRUE (ph::math::isPowerOf2(1ULL << 50));
	EXPECT_FALSE(ph::math::isPowerOf2((1ULL << 50) + 1));
	EXPECT_FALSE(ph::math::isPowerOf2(std::numeric_limits<unsigned long long>::max()));
}

TEST(MathTest, CalculateIntegerBase2Logarithm)
{
	EXPECT_EQ(ph::math::log2Floor(1), 0);
	EXPECT_EQ(ph::math::log2Floor(2), 1);
	EXPECT_EQ(ph::math::log2Floor(3), 1);
	EXPECT_EQ(ph::math::log2Floor(4), 2);
	EXPECT_EQ(ph::math::log2Floor(5), 2);
	EXPECT_EQ(ph::math::log2Floor(6), 2);
	EXPECT_EQ(ph::math::log2Floor(7), 2);
	EXPECT_EQ(ph::math::log2Floor(8), 3);

	EXPECT_EQ(ph::math::log2Floor(1024), 10);
	EXPECT_EQ(ph::math::log2Floor(1 << 20), 20);
	EXPECT_EQ(ph::math::log2Floor(std::numeric_limits<ph::uint32>::max()), 31);
}

TEST(MathTest, RetrieveFractionalPartOfANumber)
{
	EXPECT_FLOAT_EQ(ph::math::fractionalPart( 0.00f),  0.00f);
	EXPECT_FLOAT_EQ(ph::math::fractionalPart( 0.22f),  0.22f);
	EXPECT_FLOAT_EQ(ph::math::fractionalPart( 2.33f),  0.33f);
	EXPECT_FLOAT_EQ(ph::math::fractionalPart(-2.44f), -0.44f);

	EXPECT_DOUBLE_EQ(ph::math::fractionalPart( 0.00), 0.00);
	EXPECT_DOUBLE_EQ(ph::math::fractionalPart( 0.44), 0.44);
	EXPECT_DOUBLE_EQ(ph::math::fractionalPart( 3.55), 0.55);
	EXPECT_DOUBLE_EQ(ph::math::fractionalPart(-3.66),-0.66);
}

TEST(MathTest, ConstructsMatrix)
{
	auto matrix = ph::math::matrix2x2(
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

	const auto A1 = ph::math::matrix2x2(
		1.0f, 0.0f, 
		0.0f, 1.0f
	);

	const bool isX1Solved = ph::math::solveLinearSystem2x2(A1, {{1.0f, 1.0f}}, &x1);
	ASSERT_TRUE(isX1Solved);

	EXPECT_FLOAT_EQ(x1[0], 1.0f);
	EXPECT_FLOAT_EQ(x1[1], 1.0f);

	// trial 2

	std::array<float, 2> x2;

	const auto A2 = ph::math::matrix2x2(
		2.0f, 1.0f,
		1.0f, 2.0f
	);

	const bool isX2Solved = ph::math::solveLinearSystem2x2(A2, {{3.0f, 3.0f}}, &x2);
	ASSERT_TRUE(isX2Solved);

	EXPECT_FLOAT_EQ(x2[0], 1.0f);
	EXPECT_FLOAT_EQ(x2[1], 1.0f);

	// trial 3

	std::array<float, 2> x3;

	const auto A3 = ph::math::matrix2x2(
		-7.0f, -7.0f,
		-7.0f, -7.0f
	);

	const bool isX3Solved = ph::math::solveLinearSystem2x2(A3, {{14.0f, 14.0f}}, &x3);
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