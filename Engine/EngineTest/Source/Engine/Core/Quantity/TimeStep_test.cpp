#include <Engine/Core/Quantity/TimeStep.h>
#include "engine_test_util.h"

#include <gtest/gtest.h>

#include <functional>

using namespace ph;

TEST(TimeStepTest, SamplesDefaultStep)
{
	const TimeStep timeStep;
	const Time time = timeStep.sampleTime(0.25_r);

	PH_EXPECT_REAL_EQ(time.getAbsoluteS(), 0);
	PH_EXPECT_REAL_EQ(time.getStepT(), 0.25_r);
}

TEST(TimeStepTest, StoresStepInfo)
{
	const TimeStep timeStep(7, 10.0_r, 0.5_r);

	EXPECT_EQ(timeStep.getTick(), 7);
	PH_EXPECT_REAL_EQ(timeStep.getTickStartS(), 10.0_r);
	PH_EXPECT_REAL_EQ(timeStep.getStepSizeS(), 0.5_r);
}

TEST(TimeStepTest, SamplesArbitraryStep)
{
	const Time time = TimeStep(7, 10.0_r, 0.5_r).sampleTime(0.25_r);

	PH_EXPECT_REAL_EQ(time.getAbsoluteS(), 10.125_r);
	PH_EXPECT_REAL_EQ(time.getStepT(), 0.25_r);
}

TEST(TimeStepTest, ReportsDuration)
{
	EXPECT_TRUE(TimeStep(1, 2.0_r, 0.5_r).hasDuration());
	EXPECT_FALSE(TimeStep().hasDuration());
	EXPECT_FALSE(TimeStep(1, 2.0_r, 0).hasDuration());
}

TEST(TimeStepTest, HashMatchesEqualValues)
{
	const TimeStep timeStepA(2, 4.0_r, 0.5_r);
	const TimeStep timeStepB(2, 4.0_r, 0.5_r);

	EXPECT_EQ(timeStepA, timeStepB);
	EXPECT_EQ(std::hash<TimeStep>{}(timeStepA), std::hash<TimeStep>{}(timeStepB));
}
