#include <Math/Random/TPwcDistribution1D.h>

#include <gtest/gtest.h>

#include <random>

using namespace ph;

TEST(PiecewiseConstantDistribution1DTest, Construction)
{
	TPwcDistribution1D<float>  distribution1({0.0f, 1.0f, 2.0f, 0.0f, 3.0f, 0.0f});
	TPwcDistribution1D<double> distribution2({77.0});
	TPwcDistribution1D<float>  distribution3(-999.0f, 999.0f, {123.0f});
	TPwcDistribution1D<double> distribution4(1.0, 333.0, {123.0, 456.0, 789.0});
}

TEST(PiecewiseConstantDistribution1DTest, ContinuousSampleInRange)
{
	const float min = -5.0f;
	const float max = 7.0f;
	const TPwcDistribution1D<float> distribution(min, max, {0.0f, 0.0f, 1.0f, 0.0f, 2.0f, 3.0f, 0.0f});

	std::mt19937 generator(0);
	std::uniform_real_distribution<float> seedDistribution(0.0f, 1.0f);
	for(std::size_t i = 0; i < 10000; ++i)
	{
		const float seed = seedDistribution(generator);

		float sample, pdf;
		sample = distribution.sampleContinuous(seed, &pdf);

		EXPECT_GE(sample, min);
		EXPECT_LE(sample, max);
		EXPECT_GT(pdf, 0.0f);

		const std::size_t index = distribution.sampleDiscrete(seed);

		// must not sample entries with zero weight
		EXPECT_TRUE(index != 0 && index != 1 && index != 3 && index != 6);
	}
}