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

TEST(BoundTest, IntersectingTwoAABB2DsAsAreas)
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

TEST(BoundTest, IntersectingAABB2DwithPoint)
{
	typedef TAABB2D<real> AABB2DR;

	// trial 1

	const AABB2DR  aabb1(Vector2R(0, 0), Vector2R(1, 1));
	const Vector2R point1(0.5_r, 0.5_r);
	EXPECT_TRUE(aabb1.isIntersectingArea(point1));

	// trial 2

	const AABB2DR  aabb2(Vector2R(-1, -2), Vector2R(1, 1));
	const Vector2R point2(1.1_r, 0.5_r);
	EXPECT_FALSE(aabb2.isIntersectingArea(point2));

	// trial 3

	const AABB2DR  aabb3(Vector2R(-3, -3), Vector2R(3, 3));
	const Vector2R point3(-3.1_r, -3.1_r);
	EXPECT_FALSE(aabb3.isIntersectingArea(point3));
}

TEST(BoundTest, AABB2Dvalidity)
{
	typedef TAABB2D<real> AABB2DR;

	// trial 1

	const AABB2DR aabb1(Vector2R(0, 0), Vector2R(1, 1));
	EXPECT_TRUE(aabb1.isValid());

	// trial 2

	const AABB2DR aabb2(Vector2R(1, 1), Vector2R(-1, -1));
	EXPECT_FALSE(aabb2.isValid());
}

TEST(BoundTest, IsAABB2DactuallyPoint)
{
	typedef TAABB2D<real> AABB2DR;

	// trial 1

	const AABB2DR aabb1(Vector2R(0, 0), Vector2R(1, 1));
	EXPECT_FALSE(aabb1.isPoint());

	// trial 2

	const AABB2DR aabb2(Vector2R(1, 1), Vector2R(1, 1));
	EXPECT_TRUE(aabb2.isPoint());
}

TEST(BoundTest, UnionAABB2Ds)
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

TEST(BoundTest, IntersectAABB2Ds)
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

TEST(BoundTest, AABB2Dequality)
{
	const TAABB2D<int32> aabb1(TVector2<int32>(-1, -1), TVector2<int32>(1, 1));
	const TAABB2D<int32> aabb2(TVector2<int32>(-1, -1), TVector2<int32>(1, 1));
	const TAABB2D<int32> aabb3(TVector2<int32>( 0, -1), TVector2<int32>(1, 1));

	EXPECT_TRUE (aabb1.equals(aabb2));
	EXPECT_FALSE(aabb1.equals(aabb3));
}

TEST(BoundTest, AABB2DcalculateCenter)
{
	// trial 1

	const TAABB2D<int32> aabb1(TVector2<int32>(-2, -1), TVector2<int32>(1, 1));
	const auto& center1 = aabb1.calcCenter();
	EXPECT_EQ(center1.x, (-2 + 1) / 2);
	EXPECT_EQ(center1.y, (-1 + 1) / 2);

	// trial 2

	const TAABB2D<float32> aabb2(TVector2<float32>(-2, -1), TVector2<float32>(1, 1));
	const auto& center2 = aabb2.calcCenter();
	EXPECT_FLOAT_EQ(center2.x, (-2.0f + 1.0f) / 2.0f);
	EXPECT_FLOAT_EQ(center2.y, (-1.0f + 1.0f) / 2.0f);
}