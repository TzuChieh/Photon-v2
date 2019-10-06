#include "constants_for_test.h"

#include <Math/Geometry/TAABB3D.h>
#include <Math/Geometry/TAABB2D.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

#include <limits>

using namespace ph;
using namespace ph::math;

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

TEST(BoundTest, IsAABB3DActuallyPoint)
{
	AABB3D point({99, 99, 99}, {99, 99, 99});
	EXPECT_TRUE(point.isPoint());

	AABB3D volume({0, 0, 0}, {1, 1, 1});
	EXPECT_FALSE(volume.isPoint());
}

TEST(BoundTest, IsAABB3DRepresentFiniteVolume)
{
	AABB3D volume({-1, -1, -1}, {1, 1, 1});
	EXPECT_TRUE(volume.isFiniteVolume());

	AABB3D point({33, 33, 33}, {33, 33, 33});
	EXPECT_FALSE(point.isFiniteVolume());

	const real positiveInfinity = std::numeric_limits<real>::infinity();
	const real negativeInfinity = -positiveInfinity;

	AABB3D infiniteBounds1{Vector3R(negativeInfinity), Vector3R(positiveInfinity)};
	EXPECT_FALSE(infiniteBounds1.isFiniteVolume());

	AABB3D infiniteBounds2(Vector3R(negativeInfinity, 0, 0), Vector3R(positiveInfinity, 1, 1));
	EXPECT_FALSE(infiniteBounds2.isFiniteVolume());
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

TEST(BoundTest, IntersectingAABB2DWithPoint)
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

TEST(BoundTest, AABB2DValidity)
{
	typedef TAABB2D<real> AABB2DR;

	// trial 1

	const AABB2DR aabb1(Vector2R(0, 0), Vector2R(1, 1));
	EXPECT_TRUE(aabb1.isValid());

	// trial 2

	const AABB2DR aabb2(Vector2R(1, 1), Vector2R(-1, -1));
	EXPECT_FALSE(aabb2.isValid());
}

TEST(BoundTest, IsAABB2DActuallyPoint)
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

TEST(BoundTest, AABB2DCalculateAreas)
{
	// trial 1

	const TAABB2D<real> aabb1(Vector2R(-1, -1), Vector2R(1, 1));
	EXPECT_NEAR(aabb1.getArea(), 4.0_r, TEST_REAL_EPSILON);

	// trial 2

	const TAABB2D<int32> aabb2(TVector2<int32>(-3, -1), TVector2<int32>(3, 1));
	EXPECT_EQ(aabb2.getArea(), 12);
}

TEST(BoundTest, AABB2DEquality)
{
	const TAABB2D<int32> aabb1(TVector2<int32>(-1, -1), TVector2<int32>(1, 1));
	const TAABB2D<int32> aabb2(TVector2<int32>(-1, -1), TVector2<int32>(1, 1));
	const TAABB2D<int32> aabb3(TVector2<int32>( 0, -1), TVector2<int32>(1, 1));

	EXPECT_TRUE (aabb1.equals(aabb2));
	EXPECT_FALSE(aabb1.equals(aabb3));
}

TEST(BoundTest, AABB2DCalculateCenter)
{
	// trial 1

	const TAABB2D<int32> aabb1(TVector2<int32>(-2, -1), TVector2<int32>(1, 1));
	const auto& center1 = aabb1.getCenter();
	EXPECT_EQ(center1.x, (-2 + 1) / 2);
	EXPECT_EQ(center1.y, (-1 + 1) / 2);

	// trial 2

	const TAABB2D<float32> aabb2(TVector2<float32>(-2, -1), TVector2<float32>(1, 1));
	const auto& center2 = aabb2.getCenter();
	EXPECT_FLOAT_EQ(center2.x, (-2.0f + 1.0f) / 2.0f);
	EXPECT_FLOAT_EQ(center2.y, (-1.0f + 1.0f) / 2.0f);
}

// TODO: more tests
TEST(BoundTest, SplittingAABB2D)
{
	const TAABB2D<int> aabb1({0, 0}, {5, 5});
	const auto splittedAabb1 = aabb1.getSplitted(constant::Y_AXIS, 3);
	EXPECT_TRUE(splittedAabb1.first.equals(TAABB2D<int>({0, 0}, {5, 3})));
	EXPECT_TRUE(splittedAabb1.second.equals(TAABB2D<int>({0, 3}, {5, 5})));
}

// TODO: more tests
TEST(BoundTest, SplittingAABB3D)
{
	const AABB3D aabb1({-1, -1, -1}, {1, 1, 1});
	const auto splittedAabb1 = aabb1.getSplitted(constant::X_AXIS, 0);

	EXPECT_TRUE(splittedAabb1.first.getMinVertex() == aabb1.getMinVertex());
	EXPECT_EQ(splittedAabb1.first.getMaxVertex().x, 0);
	EXPECT_EQ(splittedAabb1.first.getMaxVertex().y, aabb1.getMaxVertex().y);
	EXPECT_EQ(splittedAabb1.first.getMaxVertex().z, aabb1.getMaxVertex().z);

	EXPECT_TRUE(splittedAabb1.second.getMaxVertex() == aabb1.getMaxVertex());
	EXPECT_EQ(splittedAabb1.second.getMinVertex().x, 0);
	EXPECT_EQ(splittedAabb1.second.getMinVertex().y, aabb1.getMinVertex().y);
	EXPECT_EQ(splittedAabb1.second.getMinVertex().z, aabb1.getMinVertex().z);

	const TAABB3D<int> aabb2({0, 0, 0}, {3, 3, 3});
	const auto splittedAabb2 = aabb2.getSplitted(constant::X_AXIS, 2);
	EXPECT_TRUE(splittedAabb2.first.equals(TAABB3D<int>({0, 0, 0}, {2, 3, 3})));
	EXPECT_TRUE(splittedAabb2.second.equals(TAABB3D<int>({2, 0, 0}, {3, 3, 3})));
}