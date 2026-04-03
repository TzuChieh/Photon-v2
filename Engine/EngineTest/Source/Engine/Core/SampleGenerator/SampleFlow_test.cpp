#include "engine_test_util.h"

#include <Engine/Core/SampleGenerator/SampleFlow.h>

#include <gtest/gtest.h>

#include <vector>
#include <array>

using namespace ph;

TEST(SampleFlowTest, Construction)
{
	// Default constructor
	{
		SampleFlow flow;
		// Just check if it's usable without crashing
		flow.flow1D();
	}

	// Constructor with saved dimensions
	{
		const std::vector<real> savedDims = {0.1_r, 0.2_r, 0.3_r};
		SampleFlow flow(savedDims.data(), savedDims.size());
		
		EXPECT_EQ(flow.flow1D(), 0.1_r);
		EXPECT_EQ(flow.flow1D(), 0.2_r);
		EXPECT_EQ(flow.flow1D(), 0.3_r);
	}
}

TEST(SampleFlowTest, DimensionFlowing)
{
	const std::vector<real> savedDims = {0.1_r, 0.2_r, 0.3_r, 0.4_r, 0.5_r, 0.6_r};
	SampleFlow flow(savedDims.data(), savedDims.size());

	// 1D
	EXPECT_EQ(flow.flow1D(), 0.1_r);

	// 2D
	const auto dim2 = flow.flow2D();
	EXPECT_EQ(dim2[0], 0.2_r);
	EXPECT_EQ(dim2[1], 0.3_r);

	// 3D
	const auto dim3 = flow.flow3D();
	EXPECT_EQ(dim3[0], 0.4_r);
	EXPECT_EQ(dim3[1], 0.5_r);
	EXPECT_EQ(dim3[2], 0.6_r);
}

TEST(SampleFlowTest, BasicPicking)
{
	{
		const std::vector<real> savedDims = {0.4_r};
		SampleFlow flow(savedDims.data(), savedDims.size());
		EXPECT_TRUE(flow.pick(0.5_r));
	}

	{
		const std::vector<real> savedDims = {0.6_r};
		SampleFlow flow(savedDims.data(), savedDims.size());
		EXPECT_FALSE(flow.pick(0.5_r));
	}
}

TEST(SampleFlowTest, UnflowedPickReuse)
{
	// Positive Pick
	{
		const std::vector<real> savedDims = {0.2_r, 0.9_r};
		SampleFlow flow(savedDims.data(), savedDims.size());

		// 0.2 < 0.5 is true. 
		// Remainder: 0.2 / 0.5 = 0.4
		EXPECT_TRUE(flow.unflowedPick(0.5_r));
		
		// Should return the remainder 0.4 and not consume 0.9
		PH_EXPECT_REAL_EQ(flow.flow1D(), 0.4_r);
		
		// Next one should be 0.9
		EXPECT_EQ(flow.flow1D(), 0.9_r);
	}

	// Negative Pick
	{
		const std::vector<real> savedDims = {0.8_r, 0.1_r};
		SampleFlow flow(savedDims.data(), savedDims.size());

		// 0.8 < 0.5 is false.
		// Remainder: (0.8 - 0.5) / (1.0 - 0.5) = 0.3 / 0.5 = 0.6
		EXPECT_FALSE(flow.unflowedPick(0.5_r));

		// Should return the remainder 0.6 and not consume 0.1
		PH_EXPECT_REAL_EQ(flow.flow1D(), 0.6_r);

		// Next one should be 0.1
		EXPECT_EQ(flow.flow1D(), 0.1_r);
	}
}

TEST(SampleFlowTest, UnflowedRandomPick)
{
	const std::vector<real> savedDims = {0.5_r};
	SampleFlow flow(savedDims.data(), savedDims.size());

	// Should not consume the saved dimension
	flow.unflowedRandomPick(0.1_r);
	flow.unflowedRandomPick(0.9_r);

	EXPECT_EQ(flow.flow1D(), 0.5_r);
}
