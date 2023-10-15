#include "constants_for_test.h"

#include <Math/Geometry/TAABB2D.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

#include <limits>
#include <type_traits>

using namespace ph;
using namespace ph::math;

TEST(TAABB2DTest, Requirements)
{
	EXPECT_TRUE(std::is_trivially_copyable_v<TAABB2D<short>>);
	EXPECT_TRUE(std::is_trivially_copyable_v<TAABB2D<int>>);
	EXPECT_TRUE(std::is_trivially_copyable_v<TAABB2D<float>>);
	EXPECT_TRUE(std::is_trivially_copyable_v<TAABB2D<double>>);
}

TEST(TAABB2DTest, InteractingWithEmptyBound)
{
	{
		EXPECT_TRUE(AABB2D::makeEmpty().isEmpty());// empty
		EXPECT_FALSE(AABB2D({0.0_r, 0.0_r}).isEmpty());// point
		EXPECT_FALSE(AABB2D({0.0_r, 1.0_r}).isEmpty());// area
	}

	{
		AABB2D point({123.0_r, 456.0_r});
		AABB2D empty = AABB2D::makeEmpty();
		point.unionWith(empty);
		EXPECT_TRUE(point.isPoint());
		EXPECT_FALSE(point.isArea());
		EXPECT_EQ(point.getMinVertex().x(), 123.0_r);
		EXPECT_EQ(point.getMinVertex().y(), 456.0_r);
	}

	{
		AABB2D area({-1.0_r, -1.0_r}, {1.0_r, 1.0_r});
		AABB2D empty = AABB2D::makeEmpty();
		area.unionWith(empty);
		EXPECT_FALSE(area.isPoint());
		EXPECT_TRUE(area.isArea());
		EXPECT_EQ(area.getMinVertex().x(), -1.0_r);
		EXPECT_EQ(area.getMinVertex().y(), -1.0_r);
		EXPECT_EQ(area.getMaxVertex().x(), 1.0_r);
		EXPECT_EQ(area.getMaxVertex().y(), 1.0_r);
	}

	{
		auto bound = TAABB2D<int>::makeEmpty();
		EXPECT_TRUE(bound.isEmpty());
		EXPECT_FALSE(bound.isPoint());
		EXPECT_FALSE(bound.isArea());

		for(int i = -10; i < 10; ++i)
		{
			bound.unionWith(TAABB2D<int>({i, i}, {i + 1, i + 1}));
		}
		EXPECT_TRUE(bound.isArea());
		EXPECT_EQ(bound.getMinVertex().x(), -10);
		EXPECT_EQ(bound.getMinVertex().y(), -10);
		EXPECT_EQ(bound.getMaxVertex().x(), 10);
		EXPECT_EQ(bound.getMaxVertex().y(), 10);
	}
}

TEST(TAABB2DTest, IntersectingTwoAABB2DsAsAreas)
{
	using AABB2DR = TAABB2D<real>;

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

TEST(TAABB2DTest, IntersectingAABB2DWithPoint)
{
	using AABB2DR = TAABB2D<real>;

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

TEST(TAABB2DTest, AABB2DValidity)
{
	using AABB2DR = TAABB2D<real>;

	// trial 1

	const AABB2DR aabb1(Vector2R(0, 0), Vector2R(1, 1));
	EXPECT_TRUE(!aabb1.isEmpty());

	// trial 2

	const AABB2DR aabb2(Vector2R(1, 1), Vector2R(-1, -1));
	EXPECT_FALSE(!aabb2.isEmpty());
}

TEST(TAABB2DTest, IsAABB2DActuallyPoint)
{
	using AABB2DR = TAABB2D<real>;

	// trial 1

	const AABB2DR aabb1(Vector2R(0, 0), Vector2R(1, 1));
	EXPECT_FALSE(aabb1.isPoint());

	// trial 2

	const AABB2DR aabb2(Vector2R(1, 1), Vector2R(1, 1));
	EXPECT_TRUE(aabb2.isPoint());
}

TEST(TAABB2DTest, UnionAABB2Ds)
{
	using AABB2DR = TAABB2D<real>;

	const AABB2DR aabb1(Vector2R( 0,  0), Vector2R(1, 1));
	const AABB2DR aabb2(Vector2R(-1, -1), Vector2R(2, 4));
	const AABB2DR unioned = AABB2DR(aabb1).unionWith(aabb2);
	EXPECT_NEAR(unioned.getMinVertex().x(), -1, TEST_REAL_EPSILON);
	EXPECT_NEAR(unioned.getMinVertex().y(), -1, TEST_REAL_EPSILON);
	EXPECT_NEAR(unioned.getMaxVertex().x(),  2, TEST_REAL_EPSILON);
	EXPECT_NEAR(unioned.getMaxVertex().y(),  4, TEST_REAL_EPSILON);
}

TEST(TAABB2DTest, IntersectAABB2Ds)
{
	using AABB2DR = TAABB2D<real>;

	const AABB2DR aabb1(Vector2R(-1, -1),       Vector2R(1, 1));
	const AABB2DR aabb2(Vector2R(0.5_r, 0.3_r), Vector2R(0.8_r, 1.3_r));
	const AABB2DR intersected = AABB2DR(aabb1).intersectWith(aabb2);
	EXPECT_NEAR(intersected.getMinVertex().x(), 0.5_r, TEST_REAL_EPSILON);
	EXPECT_NEAR(intersected.getMinVertex().y(), 0.3_r, TEST_REAL_EPSILON);
	EXPECT_NEAR(intersected.getMaxVertex().x(), 0.8_r, TEST_REAL_EPSILON);
	EXPECT_NEAR(intersected.getMaxVertex().y(), 1.0_r, TEST_REAL_EPSILON);
}

TEST(TAABB2DTest, AABB2DCalculateAreas)
{
	// trial 1

	const TAABB2D<real> aabb1(Vector2R(-1, -1), Vector2R(1, 1));
	EXPECT_NEAR(aabb1.getArea(), 4.0_r, TEST_REAL_EPSILON);

	// trial 2

	const TAABB2D<int32> aabb2(TVector2<int32>(-3, -1), TVector2<int32>(3, 1));
	EXPECT_EQ(aabb2.getArea(), 12);
}

TEST(TAABB2DTest, AABB2DEquality)
{
	const TAABB2D<int32> aabb1(TVector2<int32>(-1, -1), TVector2<int32>(1, 1));
	const TAABB2D<int32> aabb2(TVector2<int32>(-1, -1), TVector2<int32>(1, 1));
	const TAABB2D<int32> aabb3(TVector2<int32>( 0, -1), TVector2<int32>(1, 1));

	EXPECT_TRUE (aabb1.isEqual(aabb2));
	EXPECT_FALSE(aabb1.isEqual(aabb3));
}

TEST(TAABB2DTest, AABB2DCalculateCenter)
{
	// trial 1

	const TAABB2D<int32> aabb1(TVector2<int32>(-2, -1), TVector2<int32>(1, 1));
	const auto& center1 = aabb1.getCenter();
	EXPECT_EQ(center1.x(), (-2 + 1) / 2);
	EXPECT_EQ(center1.y(), (-1 + 1) / 2);

	// trial 2

	const TAABB2D<float32> aabb2(TVector2<float32>(-2, -1), TVector2<float32>(1, 1));
	const auto& center2 = aabb2.getCenter();
	EXPECT_FLOAT_EQ(center2.x(), (-2.0f + 1.0f) / 2.0f);
	EXPECT_FLOAT_EQ(center2.y(), (-1.0f + 1.0f) / 2.0f);
}

// TODO: more tests
TEST(TAABB2DTest, SplittingAABB2D)
{
	const TAABB2D<int> aabb1({0, 0}, {5, 5});
	const auto splittedAabb1 = aabb1.getSplitted(constant::Y_AXIS, 3);
	EXPECT_TRUE(splittedAabb1.first.isEqual(TAABB2D<int>({0, 0}, {5, 3})));
	EXPECT_TRUE(splittedAabb1.second.isEqual(TAABB2D<int>({0, 3}, {5, 5})));
}
