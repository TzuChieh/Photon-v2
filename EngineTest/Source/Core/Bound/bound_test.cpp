#include <Core/Bound/AABB3D.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(BoundTest, IntersectingTwoAABB3DsAsVolumes)
{
	// trial 1

	AABB3D aabb1a(Vector3R(0, 0, 0),             Vector3R(1, 1, 1));
	AABB3D aabb1b(Vector3R(0.8_r, 0.8_r, 0.8_r), Vector3R(1.8_r, 1.8_r, 1.8_r));
	EXPECT_TRUE(aabb1a.isIntersectingVolume(aabb1b));

	// trial 2

	AABB3D aabb2a(Vector3R(1, 1, 1),             Vector3R(3, 3, 3));
	AABB3D aabb2b(Vector3R(3.1_r, 3.1_r, 3.1_r), Vector3R(4, 4, 4));
	EXPECT_FALSE(aabb2a.isIntersectingVolume(aabb2b));

	// trial 3

	AABB3D aabb3a(Vector3R(-1, -1, -1),    Vector3R(1, 1, 1));
	AABB3D aabb3b(Vector3R(-1, -1, 1.1_r), Vector3R(1, 1, 1.2_r));
	EXPECT_FALSE(aabb3a.isIntersectingVolume(aabb3b));
}