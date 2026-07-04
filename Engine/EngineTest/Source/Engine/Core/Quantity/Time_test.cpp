#include <Engine/Core/Quantity/Time.h>
#include "engine_test_util.h"

#include <gtest/gtest.h>

using namespace ph;

TEST(TimeTest, DefaultsToTickStart)
{
	const Time time;

	PH_EXPECT_REAL_EQ(time.getAbsoluteS(), 0);
	PH_EXPECT_REAL_EQ(time.getStepT(), 0);
}

TEST(TimeTest, StoresAbsoluteTimeAndStepT)
{
	const Time time(3.5_r, 0.25_r);

	PH_EXPECT_REAL_EQ(time.getAbsoluteS(), 3.5_r);
	PH_EXPECT_REAL_EQ(time.getStepT(), 0.25_r);
}

TEST(TimeTest, LerpsAbsoluteTimeAndStepT)
{
	const Time time = Time::lerp(
		Time(2.0_r, 0.25_r),
		Time(6.0_r, 0.75_r),
		0.5_r);

	PH_EXPECT_REAL_EQ(time.getAbsoluteS(), 4.0_r);
	PH_EXPECT_REAL_EQ(time.getStepT(), 0.5_r);
}
