#include "engine_test_util.h"

#include <Engine/Core/HitDetail.h>
#include <Engine/Core/HitEvent.h>
#include <Engine/Core/Intersection/PTriangle.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(HitEventTest, OwnsRecordedRayAndHit)
{
	PTriangle triangle({0, 0, 0}, {1, 0, 0}, {0, 1, 0});
	Ray ray({0.25_r, 0.25_r, 1}, {0, 0, -1});
	HitEvent event(ray);
	EXPECT_TRUE(triangle.isIntersecting(event));
	
	const HitEvent recordedEvent = event;
	ray.setOrigin({2, 2, 1});
	event.reset(ray);
	EXPECT_FALSE(triangle.isIntersecting(event));

	HitDetail detail;
	recordedEvent.calcHitDetail(&detail);
	EXPECT_EQ(recordedEvent.getRay().getOrigin(), Vector3R(0.25_r, 0.25_r, 1));
	EXPECT_EQ(detail.getPos(), Vector3R(0.25_r, 0.25_r, 0));
	EXPECT_TRUE(recordedEvent.getReason().hasExactly(ESurfaceHitReason::IncidentRay));
}
