#include "constants_for_test.h"

#include <Core/Bound/AABB3D.h>
#include <Core/Bound/TAABB2D.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(BoundTest, IntersectingTwoAABB3DsAsVolumes)
{
	// trial 1

	const AABB3D aabb1a(Vector3R(0, 0, 0),             Vector3R(1, 1, 1));
	const AABB3D aabb1b(Vector3R(0.8_r, 0.8_r, 0.8_r), Vector3R(1.8_r, 1.8_r, 1.8_r));
	EXPECT_TRUE(aabb1a.isIntersectingVolume(aabb1b));

	// trial 2

	const AABB3D aabb2a(Vector3R(1, 1, 1),             Vector3R(3, 3, 3));
	const AABB3D aabb2b(Vector3R(3.1_r, 3.1_r, 3.1_r), Vector3R(4, 4, 4));
	EXPECT_FALSE(aabb2a.isIntersectingVolume(aabb2b));

	// trial 3

	const AABB3D aabb3a(Vector3R(-1, -1, -1),    Vector3R(1, 1, 1));
	const AABB3D aabb3b(Vector3R(-1, -1, 1.1_r), Vector3R(1, 1, 1.2_r));
	EXPECT_FALSE(aabb3a.isIntersectingVolume(aabb3b));
}

TEST(BoundTest, IntersectingTwoAABB2DRsAsAreas)
{
	typedef TAABB2D<real> AABB2DR;

	// trial 1

	const AABB2DR aabb1a(Vector2R(0, 0),         Vector2R(1, 1));
	const AABB2DR aabb1b(Vector2R(0.8_r, 0.8_r), Vector2R(1.8_r, 1.8_r));
	EXPECT_TRUE(aabb1a.isIntersectingArea(aabb1b));

	// trial 2

	const AABB2DR aabb2a(Vector2R(1, 1),         Vector2R(3, 3));
	const AABB2DR aabb2b(Vector2R(3.1_r, 3.1_r), Vector2R(4, 4));
	EXPECT_FALSE(aabb2a.isIntersectingArea(aabb2b));

	// trial 3

	const AABB2DR aabb3a(Vector2R(-1, -1),    Vector2R(1, 1));
	const AABB2DR aabb3b(Vector2R(-1, 1.1_r), Vector2R(1, 2));
	EXPECT_FALSE(aabb3a.isIntersectingArea(aabb3b));
}

TEST(BoundTest, AABB2DRvadity)
{
	typedef TAABB2D<real> AABB2DR;

	// trial 1

	const AABB2DR aabb1(Vector2R(0, 0), Vector2R(1, 1));
	EXPECT_TRUE(aabb1.isValid());

	// trial 2

	const AABB2DR aabb2(Vector2R(1, 1), Vector2R(-1, -1));
	EXPECT_FALSE(aabb2.isValid());
}

TEST(BoundTest, AABB2DRisPoint)
{
	typedef TAABB2D<real> AABB2DR;

	// trial 1

	const AABB2DR aabb1(Vector2R(0, 0), Vector2R(1, 1));
	EXPECT_FALSE(aabb1.isPoint());

	// trial 2

	const AABB2DR aabb2(Vector2R(1, 1), Vector2R(1, 1));
	EXPECT_TRUE(aabb2.isPoint());
}

TEST(BoundTest, UnionAABB2DRs)
{
	typedef TAABB2D<real> AABB2DR;

	const AABB2DR aabb1(Vector2R( 0,  0), Vector2R(1, 1));
	const AABB2DR aabb2(Vector2R(-1, -1), Vector2R(2, 4));
	const AABB2DR unioned = AABB2DR(aabb1).unionWith(aabb2);
	EXPECT_NEAR(unioned.minVertex.x, -1, TEST_REAL_EPSILON);
	EXPECT_NEAR(unioned.minVertex.y, -1, TEST_REAL_EPSILON);
	EXPECT_NEAR(unioned.maxVertex.x,  2, TEST_REAL_EPSILON);
	EXPECT_NEAR(unioned.maxVertex.y,  4, TEST_REAL_EPSILON);
}

TEST(BoundTest, IntersectAABB2DRs)
{
	typedef TAABB2D<real> AABB2DR;

	const AABB2DR aabb1(Vector2R(-1, -1),       Vector2R(1, 1));
	const AABB2DR aabb2(Vector2R(0.5_r, 0.3_r), Vector2R(0.8_r, 1.3_r));
	const AABB2DR intersected = AABB2DR(aabb1).intersectWith(aabb2);
	EXPECT_NEAR(intersected.minVertex.x, 0.5_r, TEST_REAL_EPSILON);
	EXPECT_NEAR(intersected.minVertex.y, 0.3_r, TEST_REAL_EPSILON);
	EXPECT_NEAR(intersected.maxVertex.x, 0.8_r, TEST_REAL_EPSILON);
	EXPECT_NEAR(intersected.maxVertex.y, 1.0_r, TEST_REAL_EPSILON);
}

TEST(BoundTest, AABB2DcalculateAreas)
{
	// trial 1

	const TAABB2D<real> aabb1(Vector2R(-1, -1), Vector2R(1, 1));
	EXPECT_NEAR(aabb1.calcArea(), 4.0_r, TEST_REAL_EPSILON);

	// trial 2

	const TAABB2D<int32> aabb2(TVector2<int32>(-3, -1), TVector2<int32>(3, 1));
	EXPECT_EQ(aabb2.calcArea(), 12);
}