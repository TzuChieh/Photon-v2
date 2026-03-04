#include "engine_test_constants.h"

#include <Engine/Math/TQuaternion.h>
#include <Engine/Math/TVector3.h>
#include <Engine/Math/TMatrix4.h>

#include <gtest/gtest.h>

#include <type_traits>
#include <cmath>

using namespace ph;
using namespace ph::math;

TEST(TQuaternionTest, Requirements)
{
	// Quaternions should be trivially copyable for performance in the engine
	EXPECT_TRUE(std::is_trivially_copyable_v<QuaternionR>);
	EXPECT_TRUE(std::is_trivially_copyable_v<QuaternionF>);
	EXPECT_TRUE(std::is_trivially_copyable_v<QuaternionD>);
}

TEST(TQuaternionTest, Constructs)
{
	// Basic component-wise construction
	const QuaternionR q1(1.0_r, 2.0_r, 3.0_r, 4.0_r);
	EXPECT_EQ(1.0_r, q1.x());
	EXPECT_EQ(2.0_r, q1.y());
	EXPECT_EQ(3.0_r, q1.z());
	EXPECT_EQ(4.0_r, q1.w());

	// Identity quaternion (no rotation)
	const QuaternionR q2 = QuaternionR::makeNoRotation();
	EXPECT_EQ(0.0_r, q2.x());
	EXPECT_EQ(0.0_r, q2.y());
	EXPECT_EQ(0.0_r, q2.z());
	EXPECT_EQ(1.0_r, q2.w());

	// Geometric landmark: A 180-degree rotation around the X-axis.
	// Intuitively, this "flips" the orientation. In quaternion form (axis * sin(theta/2), cos(theta/2)),
	// for 180 degrees, sin(90) = 1 and cos(90) = 0.
	const Vector3R axis(1.0_r, 0.0_r, 0.0_r);
	const real angle = to_radians(180.0_r);
	const QuaternionR q3(axis, angle);
	EXPECT_NEAR(1.0_r, q3.x(), TEST_REAL_EPSILON);
	EXPECT_NEAR(0.0_r, q3.y(), TEST_REAL_EPSILON);
	EXPECT_NEAR(0.0_r, q3.z(), TEST_REAL_EPSILON);
	EXPECT_NEAR(0.0_r, q3.w(), TEST_REAL_EPSILON);
}

TEST(TQuaternionTest, Normalize)
{
	// Non-unit quaternion
	QuaternionR q(1.0_r, 1.0_r, 1.0_r, 1.0_r);
	EXPECT_NEAR(2.0_r, q.length(), TEST_REAL_EPSILON);

	// Normalizing to unit length
	q.normalizeLocal();
	EXPECT_NEAR(1.0_r, q.length(), TEST_REAL_EPSILON);
	EXPECT_NEAR(0.5_r, q.x(), TEST_REAL_EPSILON);
	EXPECT_NEAR(0.5_r, q.y(), TEST_REAL_EPSILON);
	EXPECT_NEAR(0.5_r, q.z(), TEST_REAL_EPSILON);
	EXPECT_NEAR(0.5_r, q.w(), TEST_REAL_EPSILON);
}

TEST(TQuaternionTest, Conjugate)
{
	// Conjugate flips the sign of the vector part (x, y, z)
	const QuaternionR q(1.0_r, 2.0_r, 3.0_r, 4.0_r);
	const QuaternionR conj = q.conjugate();
	EXPECT_EQ(-1.0_r, conj.x());
	EXPECT_EQ(-2.0_r, conj.y());
	EXPECT_EQ(-3.0_r, conj.z());
	EXPECT_EQ( 4.0_r, conj.w());
}

TEST(TQuaternionTest, Mul)
{
	// Combined rotations: rotating 90 degrees around Z followed by another 90 degrees around Z.
	// Intuitively, this must equal a single 180-degree rotation around Z (0, 0, 1, 0).
	const real rad45 = to_radians(90.0_r / 2.0_r);
	const QuaternionR q90z(0.0_r, 0.0_r, std::sin(rad45), std::cos(rad45));
	const QuaternionR q180z = q90z.mul(q90z);
	EXPECT_NEAR(0.0_r, q180z.x(), TEST_REAL_EPSILON);
	EXPECT_NEAR(0.0_r, q180z.y(), TEST_REAL_EPSILON);
	EXPECT_NEAR(1.0_r, q180z.z(), TEST_REAL_EPSILON);
	EXPECT_NEAR(0.0_r, q180z.w(), TEST_REAL_EPSILON);

	// Hamilton's fundamental rules: i * j = k.
	// Multiplying the pure unit quaternion 'i' (X-axis) by 'j' (Y-axis) results in 'k' (Z-axis).
	const QuaternionR qi(1.0_r, 0.0_r, 0.0_r, 0.0_r);
	const QuaternionR qj(0.0_r, 1.0_r, 0.0_r, 0.0_r);
	const QuaternionR qk = qi.mul(qj);
	EXPECT_NEAR(0.0_r, qk.x(), TEST_REAL_EPSILON);
	EXPECT_NEAR(0.0_r, qk.y(), TEST_REAL_EPSILON);
	EXPECT_NEAR(1.0_r, qk.z(), TEST_REAL_EPSILON);
	EXPECT_NEAR(0.0_r, qk.w(), TEST_REAL_EPSILON);

	// Test the vector multiplication path: i * j = k.
	// This treats the Vector3R as a pure quaternion with w=0.
	const Vector3R vj(0.0_r, 1.0_r, 0.0_r);
	const QuaternionR qkFromVec = qi.mul(vj);
	EXPECT_NEAR(0.0_r, qkFromVec.x(), TEST_REAL_EPSILON);
	EXPECT_NEAR(0.0_r, qkFromVec.y(), TEST_REAL_EPSILON);
	EXPECT_NEAR(1.0_r, qkFromVec.z(), TEST_REAL_EPSILON);
	EXPECT_NEAR(0.0_r, qkFromVec.w(), TEST_REAL_EPSILON);
}

TEST(TQuaternionTest, ToRotationMatrix)
{
	// Rotating 180 degrees around the X-axis:
	// Intuitively, any point on the X-axis stays exactly where it is (1 -> 1).
	// Points on the Y and Z axes are flipped to the opposite side (1 -> -1).
	const QuaternionR q180x(1.0_r, 0.0_r, 0.0_r, 0.0_r);

	Matrix4R mat;
	q180x.toRotationMatrix(&mat);
	
	// Check the diagonal: X stays 1, Y becomes -1, Z becomes -1.
	EXPECT_NEAR( 1.0_r, mat.m[0][0], TEST_REAL_EPSILON);
	EXPECT_NEAR(-1.0_r, mat.m[1][1], TEST_REAL_EPSILON);
	EXPECT_NEAR(-1.0_r, mat.m[2][2], TEST_REAL_EPSILON);
	
	// All off-diagonal elements should be 0.
	EXPECT_NEAR( 0.0_r, mat.m[0][1], TEST_REAL_EPSILON);
	EXPECT_NEAR( 0.0_r, mat.m[0][2], TEST_REAL_EPSILON);
	EXPECT_NEAR( 0.0_r, mat.m[1][0], TEST_REAL_EPSILON);
	EXPECT_NEAR( 0.0_r, mat.m[1][2], TEST_REAL_EPSILON);
	EXPECT_NEAR( 0.0_r, mat.m[2][0], TEST_REAL_EPSILON);
	EXPECT_NEAR( 0.0_r, mat.m[2][1], TEST_REAL_EPSILON);
}

TEST(TQuaternionTest, Dot)
{
	// Dot product of two quaternions
	const QuaternionR q1(1.0_r, 2.0_r, 3.0_r, 4.0_r);
	const QuaternionR q2(0.5_r, 0.5_r, 0.5_r, 0.5_r);
	
	// (1*0.5 + 2*0.5 + 3*0.5 + 4*0.5) = 0.5 + 1.0 + 1.5 + 2.0 = 5.0
	EXPECT_NEAR(5.0_r, q1.dot(q2), TEST_REAL_EPSILON);
}
