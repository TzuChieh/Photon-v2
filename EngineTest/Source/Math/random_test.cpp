#include <Math/Random.h>

#include <gtest/gtest.h>

using namespace ph;

// TODO: seed with time or other data

TEST(RandomNumberTest, GeneratesRealNumberInExpectedRange)
{
	for(std::size_t i = 0; i < 512; i++)
	{
		const real value = Random::genUniformReal_i0_e1();
		EXPECT_TRUE(0.0_r <= value && value < 1.0_r);
	}
}

TEST(RandomNumberTest, GeneratesIndexInExpectedRange)
{
	const std::size_t lowerBound = 1234;
	const std::size_t upperBound = 765432;
	for(std::size_t i = 0; i < 512; i++)
	{
		const std::size_t index = Random::genUniformIndex_iL_eU(lowerBound, upperBound);
		EXPECT_TRUE(lowerBound <= index && index < upperBound);
	}
}