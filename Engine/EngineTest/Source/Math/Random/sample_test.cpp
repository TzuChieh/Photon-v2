#include <Math/Random/sample.h>

#include <gtest/gtest.h>

using namespace ph::math;

TEST(SampleUtilityTest, BinaryPick)
{
	{
		const float sample = 0.4f;
		EXPECT_TRUE(pick(0.6f, sample));
	}

	{
		const float sample = 0.99f;
		EXPECT_FALSE(pick(0.98f, sample));
	}

	{
		const float sample = 0.101f;
		EXPECT_TRUE(pick(1.0f, sample));
	}

	{
		const float sample = 0.123f;
		EXPECT_FALSE(pick(0.0f, sample));
	}

	// never pick up a 0 probability event even with a 0-sample
	{
		const float sample = 0.0f;
		EXPECT_FALSE(pick(0.0f, sample));
	}
}

TEST(SampleUtilityTest, BinaryPickReused)
{
	{
		float sample = 0.2f;
		EXPECT_TRUE(reused_pick(0.5f, sample));
		EXPECT_FLOAT_EQ(sample, 0.4f);
	}

	{
		float sample = 0.8f;
		EXPECT_FALSE(reused_pick(0.6f, sample));
		EXPECT_FLOAT_EQ(sample, 0.5f);
	}

	// a 0 probability event consumes no sample--sample is not scaled
	{
		{
			float sample = 0.456f;
			EXPECT_FALSE(reused_pick(0.0f, sample));
			EXPECT_FLOAT_EQ(sample, 0.456f);
		}

		// never pick up a 0 probability event even with a 0-sample
		{
			float sample = 0.0f;
			EXPECT_FALSE(reused_pick(0.0f, sample));
			EXPECT_FLOAT_EQ(sample, 0.0f);
		}
	}

	// a 1 probability event consumes no sample, too--sample is not scaled
	{
		float sample = 0.777f;
		EXPECT_TRUE(reused_pick(1.0f, sample));
		EXPECT_FLOAT_EQ(sample, 0.777f);
	}
}
