#include <Math/Random/TPwcDistribution1D.h>
#include <Math/Random/TPwcDistribution2D.h>

#include <gtest/gtest.h>

#include <random>
#include <cmath>

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

TEST(PiecewiseConstantDistribution2DTest, Construction)
{
	std::vector<float> weights = {1, 2, 3, 4};
	TPwcDistribution2D<float> distribution(weights.data(), {2, 2});
}

TEST(PiecewiseConstantDistribution2DTest, ContinuousSampleInRange)
{
	std::vector<float> weights = 
	{
		0, 0, 1, 
		1, 2, 1,
		0, 0, 0
	};
	const TPwcDistribution2D<float> distribution(weights.data(), {3, 3});

	//std::vector<std::size_t> counts(weights.size(), 0);

	std::mt19937 generator(0);
	std::uniform_real_distribution<float> seedDistribution(0.0f, 1.0f);
	for(std::size_t i = 0; i < 10000; ++i)
	{
		const float seedX = seedDistribution(generator);
		const float seedY = seedDistribution(generator);

		float pdf;
		TVector2<float> sample;
		sample = distribution.sampleContinuous(seedX, seedY, &pdf);
		
		EXPECT_GE(sample.x, 0.0f);
		EXPECT_LE(sample.x, 1.0f);
		EXPECT_GE(sample.y, 0.0f);
		EXPECT_LE(sample.y, 1.0f);
		EXPECT_GT(pdf, 0.0f);

		/*const TVector2<float> floatIndex = sample.mul(3.0f);
		const std::size_t x = std::min(static_cast<std::size_t>(floatIndex.x), 
		                               static_cast<std::size_t>(2));
		const std::size_t y = std::min(static_cast<std::size_t>(floatIndex.y), 
		                               static_cast<std::size_t>(2));
		counts[y * 3 + x]++;*/
	}

	/*for(auto count : counts)
	{
		std::cout << count << std::endl;
	}*/
}