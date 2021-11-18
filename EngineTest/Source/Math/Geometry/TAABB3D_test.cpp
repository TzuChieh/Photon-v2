#include "constants_for_test.h"

#include <Math/Geometry/TAABB3D.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

#include <limits>
#include <type_traits>

using namespace ph;
using namespace ph::math;

TEST(TAABB3DTest, Requirements)
{
	EXPECT_TRUE(std::is_trivially_copyable_v<TAABB3D<short>>);
	EXPECT_TRUE(std::is_trivially_copyable_v<TAABB3D<int>>);
	EXPECT_TRUE(std::is_trivially_copyable_v<TAABB3D<float>>);
	EXPECT_TRUE(std::is_trivially_copyable_v<TAABB3D<double>>);
}

TEST(TAABB3DTest, IntersectingTwoAABB3DsAsVolumes)
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

TEST(TAABB3DTest, IsAABB3DActuallyPoint)
{
	AABB3D point({99, 99, 99}, {99, 99, 99});
	EXPECT_TRUE(point.isPoint());

	AABB3D volume({0, 0, 0}, {1, 1, 1});
	EXPECT_FALSE(volume.isPoint());
}

TEST(TAABB3DTest, IsAABB3DRepresentFiniteVolume)
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

// TODO: more tests
TEST(TAABB3DTest, SplittingAABB3D)
{
	const AABB3D aabb1({-1, -1, -1}, {1, 1, 1});
	const auto splittedAabb1 = aabb1.getSplitted(constant::X_AXIS, 0);

	EXPECT_TRUE(splittedAabb1.first.getMinVertex() == aabb1.getMinVertex());
	EXPECT_EQ(splittedAabb1.first.getMaxVertex().x(), 0);
	EXPECT_EQ(splittedAabb1.first.getMaxVertex().y(), aabb1.getMaxVertex().y());
	EXPECT_EQ(splittedAabb1.first.getMaxVertex().z(), aabb1.getMaxVertex().z());

	EXPECT_TRUE(splittedAabb1.second.getMaxVertex() == aabb1.getMaxVertex());
	EXPECT_EQ(splittedAabb1.second.getMinVertex().x(), 0);
	EXPECT_EQ(splittedAabb1.second.getMinVertex().y(), aabb1.getMinVertex().y());
	EXPECT_EQ(splittedAabb1.second.getMinVertex().z(), aabb1.getMinVertex().z());

	const TAABB3D<int> aabb2({0, 0, 0}, {3, 3, 3});
	const auto splittedAabb2 = aabb2.getSplitted(constant::X_AXIS, 2);
	EXPECT_TRUE(splittedAabb2.first.isEqual(TAABB3D<int>({0, 0, 0}, {2, 3, 3})));
	EXPECT_TRUE(splittedAabb2.second.isEqual(TAABB3D<int>({2, 0, 0}, {3, 3, 3})));
}
