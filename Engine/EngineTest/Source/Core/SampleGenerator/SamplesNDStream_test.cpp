#include <Core/SampleGenerator/SamplesNDStream.h>

#include <gtest/gtest.h>

#include <vector>

using namespace ph;

TEST(SamplesNDStreamTest, RetrieveSamples)
{
	{
		const std::vector<real> buffer = {
			-4.0_r, -3.0_r,
			-2.0_r, -1.0_r,
			 1.0_r,  2.0_r,
			 3.0_r,  4.0_r,
			 5.0_r,  6.0_r,
			 7.0_r,  8.0_r,
			 0.0_r,  9.0_r
		};

		SamplesNDStream stream(buffer.data(), 2, 7);
		const real* sample = nullptr;

		sample = stream.readSample();
		EXPECT_EQ(sample[0], -4.0_r); EXPECT_EQ(sample[1], -3.0_r);

		sample = stream.readSample();
		EXPECT_EQ(sample[0], -2.0_r); EXPECT_EQ(sample[1], -1.0_r);

		sample = stream.readSample();
		EXPECT_EQ(sample[0],  1.0_r); EXPECT_EQ(sample[1],  2.0_r);

		sample = stream.readSample();
		EXPECT_EQ(sample[0],  3.0_r); EXPECT_EQ(sample[1],  4.0_r);

		sample = stream.readSample();
		EXPECT_EQ(sample[0],  5.0_r); EXPECT_EQ(sample[1],  6.0_r);

		sample = stream.readSample();
		EXPECT_EQ(sample[0],  7.0_r); EXPECT_EQ(sample[1],  8.0_r);

		sample = stream.readSample();
		EXPECT_EQ(sample[0],  0.0_r); EXPECT_EQ(sample[1],  9.0_r);
	}
}
