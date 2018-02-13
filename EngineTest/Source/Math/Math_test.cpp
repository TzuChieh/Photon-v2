#include <Math/Math.h>

#include <gtest/gtest.h>

#include <limits>

TEST(MathTest, AngleUnitConversion)
{
	EXPECT_FLOAT_EQ(ph::Math::toDegrees(3.14159265359f), 180.0f);
	EXPECT_FLOAT_EQ(ph::Math::toRadians(180.0f), 3.14159265359f);
}

TEST(MathTest, NumberSignExtraction)
{
	EXPECT_TRUE(ph::Math::sign(-3) == -1);
	EXPECT_TRUE(ph::Math::sign( 0) ==  0);
	EXPECT_TRUE(ph::Math::sign( 2) ==  1);

	EXPECT_TRUE(ph::Math::sign(-4.0f) == -1);
	EXPECT_TRUE(ph::Math::sign( 0.0f) ==  0);
	EXPECT_TRUE(ph::Math::sign( 7.0f) ==  1);

	EXPECT_TRUE(ph::Math::sign(0ULL) == 0);
	EXPECT_TRUE(ph::Math::sign(std::numeric_limits<unsigned long long>::max()) == 1);
}

TEST(MathTest, NumberClamping)
{
	EXPECT_TRUE(ph::Math::clamp(-1, 0, 4) == 0);
	EXPECT_TRUE(ph::Math::clamp( 2, 0, 4) == 2);
	EXPECT_TRUE(ph::Math::clamp( 7, 0, 4) == 4);

	EXPECT_TRUE(ph::Math::clamp(-3.0f, 1.0f, 5.0f) == 1.0f);
	EXPECT_TRUE(ph::Math::clamp( 2.5f, 1.0f, 5.0f) == 2.5f);
	EXPECT_TRUE(ph::Math::clamp( 5.1f, 1.0f, 5.0f) == 5.0f);

	if(std::numeric_limits<float>::has_quiet_NaN)
	{
		EXPECT_TRUE(ph::Math::clamp(std::numeric_limits<float>::quiet_NaN(), 1.0f, 5.0f) == 1.0f);
	}

	if(std::numeric_limits<double>::has_quiet_NaN)
	{
		EXPECT_TRUE(ph::Math::clamp(std::numeric_limits<double>::quiet_NaN(), 1.0, 5.0) == 1.0);
	}
}

TEST(MathTest, EvaluateNextPowerOf2)
{
	EXPECT_EQ(ph::Math::nextPowerOf2(1), 1);
	EXPECT_EQ(ph::Math::nextPowerOf2(2), 2);
	EXPECT_EQ(ph::Math::nextPowerOf2(3), 4);
	EXPECT_EQ(ph::Math::nextPowerOf2(4), 4);
	EXPECT_EQ(ph::Math::nextPowerOf2(5), 8);
	EXPECT_EQ(ph::Math::nextPowerOf2(6), 8);
	EXPECT_EQ(ph::Math::nextPowerOf2(7), 8);
	EXPECT_EQ(ph::Math::nextPowerOf2(8), 8);

	// TODO: test by calculating using log functions

	EXPECT_EQ(ph::Math::nextPowerOf2(1023),  1024);
	EXPECT_EQ(ph::Math::nextPowerOf2(32700), 32768);

	// special case (this behavior is part of spec.)
	EXPECT_EQ(ph::Math::nextPowerOf2(0), 0);
}

TEST(MathTest, CheckIsPowerOf2)
{
	EXPECT_FALSE(ph::Math::isPowerOf2(0));
	EXPECT_TRUE (ph::Math::isPowerOf2(1));
	EXPECT_TRUE (ph::Math::isPowerOf2(2));
	EXPECT_FALSE(ph::Math::isPowerOf2(3));
	EXPECT_TRUE (ph::Math::isPowerOf2(1 << 17));
	EXPECT_FALSE(ph::Math::isPowerOf2((1 << 17) + 1));
	EXPECT_FALSE(ph::Math::isPowerOf2(-1));
	EXPECT_FALSE(ph::Math::isPowerOf2(std::numeric_limits<int>::max()));
	EXPECT_FALSE(ph::Math::isPowerOf2(std::numeric_limits<int>::min()));
	EXPECT_FALSE(ph::Math::isPowerOf2(std::numeric_limits<unsigned int>::max()));
	EXPECT_FALSE(ph::Math::isPowerOf2(0ULL));
	EXPECT_TRUE (ph::Math::isPowerOf2(1ULL << 50));
	EXPECT_FALSE(ph::Math::isPowerOf2((1ULL << 50) + 1));
	EXPECT_FALSE(ph::Math::isPowerOf2(std::numeric_limits<unsigned long long>::max()));
}

TEST(MathTest, CalculateIntegerBase2Logarithm)
{
	EXPECT_EQ(ph::Math::log2Floor(1), 0);
	EXPECT_EQ(ph::Math::log2Floor(2), 1);
	EXPECT_EQ(ph::Math::log2Floor(3), 1);
	EXPECT_EQ(ph::Math::log2Floor(4), 2);
	EXPECT_EQ(ph::Math::log2Floor(5), 2);
	EXPECT_EQ(ph::Math::log2Floor(6), 2);
	EXPECT_EQ(ph::Math::log2Floor(7), 2);
	EXPECT_EQ(ph::Math::log2Floor(8), 3);

	EXPECT_EQ(ph::Math::log2Floor(1024), 10);
	EXPECT_EQ(ph::Math::log2Floor(1 << 20), 20);
	EXPECT_EQ(ph::Math::log2Floor(std::numeric_limits<ph::uint32>::max()), 31);
}

TEST(MathTest, RetrieveFractionalPartOfANumber)
{
	EXPECT_FLOAT_EQ(ph::Math::fractionalPart( 0.00f),  0.00f);
	EXPECT_FLOAT_EQ(ph::Math::fractionalPart( 0.22f),  0.22f);
	EXPECT_FLOAT_EQ(ph::Math::fractionalPart( 2.33f),  0.33f);
	EXPECT_FLOAT_EQ(ph::Math::fractionalPart(-2.44f), -0.44f);

	EXPECT_DOUBLE_EQ(ph::Math::fractionalPart( 0.00), 0.00);
	EXPECT_DOUBLE_EQ(ph::Math::fractionalPart( 0.44), 0.44);
	EXPECT_DOUBLE_EQ(ph::Math::fractionalPart( 3.55), 0.55);
	EXPECT_DOUBLE_EQ(ph::Math::fractionalPart(-3.66),-0.66);
}