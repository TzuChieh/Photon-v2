#include "engine_test_constants.h"

#include <Engine/Core/Quantity/Time.h>
#include <Engine/Core/Transform/StaticAffineTransform.h>
#include <Engine/Core/Transform/Transform.h>
#include <Engine/Math/Geometry/TAABB3D.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(TransformTest, SweepAabbPreservesStaticBound)
{
	const Transform& transform = StaticAffineTransform::IDENTITY();
	const AABB3D localAABB({0, 0, 0}, {2, 4, 8});

	AABB3D sweepAABB;
	transform.calcSweepAABB(
		localAABB,
		Time(0, 0),
		Time(1, 1),
		2,
		1.0_r,
		&sweepAABB);

	EXPECT_TRUE(sweepAABB.isEqual(localAABB));
}

TEST(TransformTest, SweepAabbPaddingExpandsBound)
{
	const Transform& transform = StaticAffineTransform::IDENTITY();
	const AABB3D localAABB({0, 0, 0}, {2, 4, 8});

	AABB3D paddedAABB;
	transform.calcSweepAABB(
		localAABB,
		Time(0, 0),
		Time(0, 0),
		2,
		1.25_r,
		&paddedAABB);

	EXPECT_LT(paddedAABB.getMinVertex().x(), localAABB.getMinVertex().x());
	EXPECT_GT(paddedAABB.getMaxVertex().x(), localAABB.getMaxVertex().x());
}
