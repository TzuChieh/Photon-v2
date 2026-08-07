#include "engine_test_util.h"

#include <Engine/Actor/Geometry/GCuboid.h>
#include <Engine/Core/HitProbe.h>
#include <Engine/Core/Intersection/BruteForceIntersector.h>
#include <Engine/Core/Intersection/Intersectable.h>
#include <Engine/Core/Intersection/TMaskedIntersectable.h>
#include <Engine/Core/Ray.h>
#include <Engine/Core/SurfaceHit.h>
#include <Engine/SDL/TSdl.h>
#include <Engine/World/Foundation/CookedGeometry.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/CookingContext.h>

#include <gtest/gtest.h>

#include <vector>

using namespace ph;

TEST(MaskedIntersectableTest, FindsNextUnmaskedCuboidHit)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	// z bounds = [-0.5, 0.5]
	auto cuboid = TSdl<GCuboid>::makeResource();
	cuboid->setSize(1);

	CookedGeometry* const cookedCuboid = resources.makeGeometry(ctx.getKey(cuboid));
	ASSERT_NE(cookedCuboid, nullptr);
	cuboid->cook(ctx, *cookedCuboid);

	const std::vector<const Intersectable*> cuboidFaces(
		cookedCuboid->primitives.begin(), cookedCuboid->primitives.end());
	BruteForceIntersector cuboidAggregate;
	cuboidAggregate.update(cuboidFaces);

	// z = -0.5 face is not masked off
	const auto mask = [](const SurfaceHit& X)
	{
		return X.getPos().z() < 0 ? 1.0_r : 0.0_r;
	};
	TMaskedIntersectable maskedCuboid(&cuboidAggregate, mask);

	HitProbe probe;
	const Ray ray({0.2_r, 0.1_r, 2.0_r}, {0, 0, -1});

	// Ray from z = 2 must hit the z = -0.5 face (t = 2.5)
	ASSERT_TRUE(maskedCuboid.isIntersecting(ray, probe));
	PH_EXPECT_REAL_EQ(2.5_r, probe.getHitRayT());
}
