#include <Engine/Core/Ray.h>
#include <Engine/Core/Quantity/Time.h>
#include "engine_test_util.h"

#include <gtest/gtest.h>

#include <limits>

using namespace ph;
using namespace ph::math;

TEST(RayTest, Constructs)
{
	const Vector3R origin(1, 2, 3);
	const Vector3R direction(0, 1, 0);
	const real minT = 0.5_r;
	const real maxT = 100.0_r;
	const Time time(1.0_r, 2.0_r, 0.5_r);

	// Construct with origin and direction, default range and time.
	// This should result in a ray with maximum finite length.
	{
		const Ray ray(origin, direction);
		EXPECT_EQ(ray.getOrigin(), origin);
		EXPECT_EQ(ray.getDir(), direction);
		PH_EXPECT_REAL_EQ(ray.getMinT(), 0.0_r);
		PH_EXPECT_REAL_EQ(ray.getMaxT(), std::numeric_limits<real>::max());
		EXPECT_EQ(ray.getTime(), Time{});
	}

	// Construct with origin, direction, and time
	{
		const Ray ray(origin, direction, time);
		EXPECT_EQ(ray.getOrigin(), origin);
		EXPECT_EQ(ray.getDir(), direction);
		EXPECT_EQ(ray.getTime(), time);
	}

	// Construct with origin, direction, and parametric range
	{
		const Ray ray(origin, direction, minT, maxT);
		EXPECT_EQ(ray.getOrigin(), origin);
		EXPECT_EQ(ray.getDir(), direction);
		PH_EXPECT_REAL_EQ(ray.getMinT(), minT);
		PH_EXPECT_REAL_EQ(ray.getMaxT(), maxT);
	}

	// Full constructor with time
	{
		const Ray ray(origin, direction, minT, maxT, time);
		EXPECT_EQ(ray.getOrigin(), origin);
		EXPECT_EQ(ray.getDir(), direction);
		PH_EXPECT_REAL_EQ(ray.getMinT(), minT);
		PH_EXPECT_REAL_EQ(ray.getMaxT(), maxT);
		EXPECT_EQ(ray.getTime(), time);
	}
}

TEST(RayTest, SettersAndGetters)
{
	Ray ray;
	const Vector3R origin(1, 1, 1);
	const Vector3R direction(0, 0, -1);
	const real minT = 1.0_r;
	const real maxT = 2.0_r;
	const Time time(10.0_r, 0.1_r, 0.2_r);

	// Verifying origin setting and getting
	ray.setOrigin(origin);
	EXPECT_EQ(ray.getOrigin(), origin);

	// Verifying direction setting and getting
	ray.setDir(direction);
	EXPECT_EQ(ray.getDir(), direction);

	// Verifying parametric distance limits
	ray.setMinT(minT);
	PH_EXPECT_REAL_EQ(ray.getMinT(), minT);

	ray.setMaxT(maxT);
	PH_EXPECT_REAL_EQ(ray.getMaxT(), maxT);

	// Verifying setting both parametric range bounds
	ray.setRange(0.0_r, 5.0_r);
	PH_EXPECT_REAL_EQ(ray.getMinT(), 0.0_r);
	PH_EXPECT_REAL_EQ(ray.getMaxT(), 5.0_r);

	// Verifying time attribute management
	ray.setTime(time);
	EXPECT_EQ(ray.getTime(), time);
}

TEST(RayTest, Reverse)
{
	const Vector3R origin(0, 0, 0);
	const Vector3R direction(1, 0, 0);
	Ray ray(origin, direction);

	// Reversing should negate the direction while keeping origin and range intact
	ray.reverse();
	EXPECT_EQ(ray.getOrigin(), origin);
	EXPECT_EQ(ray.getDir(), Vector3R(-1, 0, 0));
	PH_EXPECT_REAL_EQ(ray.getMinT(), 0.0_r);
}

TEST(RayTest, HeadAndTail)
{
	const Vector3R origin(1, 1, 1);
	const Vector3R direction(1, 0, 0);
	const real minT = 1.0_r;
	const real maxT = 2.0_r;
	const Ray ray(origin, direction, minT, maxT);

	// Tail position calculation: origin + minT * direction
	EXPECT_EQ(ray.getTail(), Vector3R(2, 1, 1));

	// Head position calculation: origin + maxT * direction
	EXPECT_EQ(ray.getHead(), Vector3R(3, 1, 1));
}
