#include <Engine/Math/Random/sample.h>
#include <Engine/Math/Random/Random.h>

#include <gtest/gtest.h>

#include <cstddef>
#include <cmath>

using namespace ph;

TEST(MathSampleTest, BinaryPick)
{
	{
		const float sample = 0.4f;
		EXPECT_TRUE(math::pick(0.6f, sample));
	}

	{
		const float sample = 0.99f;
		EXPECT_FALSE(math::pick(0.98f, sample));
	}

	{
		const float sample = 0.101f;
		EXPECT_TRUE(math::pick(1.0f, sample));
	}

	{
		const float sample = 0.123f;
		EXPECT_FALSE(math::pick(0.0f, sample));
	}

	// never pick up a 0 probability event even with a 0-sample
	{
		const float sample = 0.0f;
		EXPECT_FALSE(math::pick(0.0f, sample));
	}
}

TEST(MathSampleTest, BinaryPickReused)
{
	{
		float sample = 0.2f;
		EXPECT_TRUE(math::reused_pick(0.5f, sample));
		EXPECT_FLOAT_EQ(sample, 0.4f);
	}

	{
		float sample = 0.8f;
		EXPECT_FALSE(math::reused_pick(0.6f, sample));
		EXPECT_FLOAT_EQ(sample, 0.5f);
	}

	// a 0 probability event consumes no sample--sample is not scaled
	{
		{
			float sample = 0.456f;
			EXPECT_FALSE(math::reused_pick(0.0f, sample));
			EXPECT_FLOAT_EQ(sample, 0.456f);
		}

		// never pick up a 0 probability event even with a 0-sample
		{
			float sample = 0.0f;
			EXPECT_FALSE(math::reused_pick(0.0f, sample));
			EXPECT_FLOAT_EQ(sample, 0.0f);
		}
	}

	// a 1 probability event consumes no sample, too--sample is not scaled
	{
		float sample = 0.777f;
		EXPECT_TRUE(math::reused_pick(1.0f, sample));
		EXPECT_FLOAT_EQ(sample, 0.777f);
	}
}

TEST(MathSampleTest, UniformPick)
{
	constexpr std::size_t binSize = 10;
	constexpr std::size_t numSamples = 65536;
	constexpr double maxRelativeError = 0.05;// 5%

	// Count samples in each bin
	std::size_t histogram[binSize] = {};
	for(std::size_t si = 0; si < numSamples; ++si)
	{
		std::size_t idxStart = 0;
		const auto pickedIdx = math::uniform_pick<real, std::size_t>(
			[&idxStart]()
			{
				return idxStart < binSize ? std::optional<std::size_t>{idxStart++} : std::optional<std::size_t>{};
			},
			[](real prob)
			{
				return math::pick(prob, math::Random::sample());
			});

		if(pickedIdx.has_value())
		{
			EXPECT_LT(*pickedIdx, binSize);
			++histogram[*pickedIdx];
		}
	}

	// Test relatvie error in each bin
	for(std::size_t bi = 0; bi < binSize; ++bi)
	{
		const double expectedCount = static_cast<double>(numSamples) / static_cast<double>(binSize);
		const double relativeError = std::abs(static_cast<double>(histogram[bi]) - expectedCount) / expectedCount;
		EXPECT_LE(relativeError, maxRelativeError) << "bin index: " << bi << ", error: " << relativeError;
	}
}
