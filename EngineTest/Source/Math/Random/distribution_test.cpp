#include <Math/Random/TPwcDistribution1D.h>
#include <Math/Random/TPwcDistribution2D.h>

#include <gtest/gtest.h>

#include <random>
#include <cmath>

using namespace ph;
using namespace ph::math;

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

TEST(PiecewiseConstantDistribution1DTest, PDF)
{
	TPwcDistribution1D<float> distribution1({1.0f, 1.0f});
	TPwcDistribution1D<float> distribution2({2.0f, 2.0f, 2.0f});

	// constant distribution should have same continuous PDF everywhere
	EXPECT_FLOAT_EQ(distribution1.pdfContinuous(0.0f),   1.0f);
	EXPECT_FLOAT_EQ(distribution1.pdfContinuous(0.3f),   1.0f);
	EXPECT_FLOAT_EQ(distribution1.pdfContinuous(0.999f), 1.0f);
	EXPECT_FLOAT_EQ(distribution1.pdfContinuous(1.0f),   1.0f);

	// two constant distributions should have same continuous PDF everywhere
	EXPECT_FLOAT_EQ(distribution1.pdfContinuous(0.1f),   distribution2.pdfContinuous(0.1f));
	EXPECT_FLOAT_EQ(distribution1.pdfContinuous(0.1f),   distribution2.pdfContinuous(0.2f));
	EXPECT_FLOAT_EQ(distribution1.pdfContinuous(0.777f), distribution2.pdfContinuous(0.9f));
	EXPECT_FLOAT_EQ(distribution1.pdfContinuous(1.0f),   distribution2.pdfContinuous(0.55f));

	// distribution 1 has 2 equal-height columns, discrete PDF should be 1/2
	EXPECT_FLOAT_EQ(distribution1.pdfDiscrete(0), 1.0f / 2.0f);
	EXPECT_FLOAT_EQ(distribution1.pdfDiscrete(1), 1.0f / 2.0f);

	// distribution 2 has 3 equal-height columns, discrete PDF should be 1/3
	EXPECT_FLOAT_EQ(distribution2.pdfDiscrete(0), 1.0f / 3.0f);
	EXPECT_FLOAT_EQ(distribution2.pdfDiscrete(1), 1.0f / 3.0f);
	EXPECT_FLOAT_EQ(distribution2.pdfDiscrete(2), 1.0f / 3.0f);

	// single weight constant distribution has same PDF everywhere
	TPwcDistribution1D<float> distribution3({99999.0f});
	EXPECT_FLOAT_EQ(distribution3.pdfContinuous(0.0f), 1.0f);
	EXPECT_FLOAT_EQ(distribution3.pdfContinuous(0.5f), 1.0f);
	EXPECT_FLOAT_EQ(distribution3.pdfContinuous(1.0f), 1.0f);
	EXPECT_FLOAT_EQ(distribution3.pdfDiscrete(0), 1.0f);
}

TEST(PiecewiseConstantDistribution1DTest, CornerCases)
{
	TPwcDistribution1D<float> zeroWeightDistribution1({0.0f});

	// should result in uniform distribution
	EXPECT_FLOAT_EQ(zeroWeightDistribution1.pdfDiscrete(0), 1.0f);

	TPwcDistribution1D<float> zeroWeightDistribution2({0.0f, 0.0f, 0.0f});

	// should result in uniform distribution
	EXPECT_FLOAT_EQ(zeroWeightDistribution2.pdfDiscrete(0), 1.0f / 3.0f);
	EXPECT_FLOAT_EQ(zeroWeightDistribution2.pdfDiscrete(1), 1.0f / 3.0f);
	EXPECT_FLOAT_EQ(zeroWeightDistribution2.pdfDiscrete(2), 1.0f / 3.0f);
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
		const float sampleX = seedDistribution(generator);
		const float sampleY = seedDistribution(generator);

		float pdf;
		TVector2<float> value;
		value = distribution.sampleContinuous({sampleX, sampleY}, &pdf);
		
		EXPECT_GE(value.x, 0.0f);
		EXPECT_LE(value.x, 1.0f);
		EXPECT_GE(value.y, 0.0f);
		EXPECT_LE(value.y, 1.0f);
		EXPECT_GT(pdf, 0.0f);

		/*const TVector2<float> floatIndex = value.mul(3.0f);
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

TEST(PiecewiseConstantDistribution2DTest, PDF)
{
	std::vector<float> weights1 = 
	{
		1
	};
	std::vector<float> weights2 =
	{
		3, 3,
		3, 3
	};
	TPwcDistribution2D<float> distribution1(weights1.data(), {1, 1});
	TPwcDistribution2D<float> distribution2(weights2.data(), {2, 2});

	// constant distribution should have same PDF everywhere
	EXPECT_FLOAT_EQ(distribution1.pdfContinuous({0.0f,  0.0f}),  1.0f);
	EXPECT_FLOAT_EQ(distribution1.pdfContinuous({0.2f,  0.5f}),  1.0f);
	EXPECT_FLOAT_EQ(distribution1.pdfContinuous({0.99f, 0.01f}), 1.0f);
	EXPECT_FLOAT_EQ(distribution1.pdfContinuous({1.0f,  1.0f}),  1.0f);

	// two constant distributions should have same PDF everywhere
	EXPECT_FLOAT_EQ(distribution1.pdfContinuous({0.3f,   0.3f}), distribution2.pdfContinuous({0.3f,   0.3f}));
	EXPECT_FLOAT_EQ(distribution1.pdfContinuous({0.666f, 0.1f}), distribution2.pdfContinuous({0.357f, 0.432f}));
	EXPECT_FLOAT_EQ(distribution1.pdfContinuous({0.0f,   0.0f}), distribution2.pdfContinuous({1.0f,   1.0f}));
	EXPECT_FLOAT_EQ(distribution1.pdfContinuous({0.8f,   0.2f}), distribution2.pdfContinuous({0.0f,   1.0f}));
}
