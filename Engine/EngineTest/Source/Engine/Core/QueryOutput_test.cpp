#include "engine_test_util.h"

#include <Engine/Core/Emitter/Query/EnergyEmissionSampleQuery.h>
#include <Engine/Core/Intersection/PLatLong01Sphere.h>
#include <Engine/Core/Intersection/Query/PrimitivePosSampleQuery.h>
#include <Engine/Core/LTA/PDF.h>
#include <Engine/Core/Ray.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(QueryOutputTest, EmissionEventUsesEmittedRay)
{
	PLatLong01Sphere sphere(1);
	PrimitivePosSampleOutput position;
	position.setPos({0, 0, 1});
	position.setPdfPos(lta::PDF::A(1));
	position.setSampledHit(sphere, Ray({0, 0, 2}, {0, 0, -1}, 0, 1));

	// The recorded hit uses the emitted ray's minimum t.
	const Ray emittedRay(position.getPos(), {0, 0, 1}, 0.125_r, 10);
	EnergyEmissionSampleOutput emission;
	emission.setHitEvent(position, emittedRay);

	// The event is available before publishing the emission PDFs.
	const auto& event = emission.event();
	EXPECT_EQ(event.getTopHit(), &sphere);
	EXPECT_EQ(event.getRay().getOrigin(), emittedRay.getOrigin());
	EXPECT_EQ(event.getRay().getDir(), emittedRay.getDir());
	PH_EXPECT_REAL_EQ(event.getHitRayT(), 0.125_r);
	EXPECT_TRUE(event.getReason().hasExactly(ESurfaceHitReason::SampledPosDir));
}
