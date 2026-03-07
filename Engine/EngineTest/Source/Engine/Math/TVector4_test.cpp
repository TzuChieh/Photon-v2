#include "engine_test_constants.h"
#include "engine_test_util.h"

#include <Engine/Math/TVector4.h>

#include <gtest/gtest.h>

#include <type_traits>
#include <cmath>

using namespace ph;
using namespace ph::math;

TEST(TVector4Test, Requirements)
{
	EXPECT_TRUE(std::is_trivially_copyable_v<Vector4R>);
	EXPECT_TRUE(std::is_trivially_copyable_v<Vector4F>);
	EXPECT_TRUE(std::is_trivially_copyable_v<Vector4D>);
	EXPECT_TRUE(std::is_trivially_copyable_v<Vector4S>);
}

TEST(TVector4Test, Constructs)
{
	const Vector4R vec1(-1.1_r, 0.0_r, 2.3_r, 4.5_r);
	PH_EXPECT_REAL_EQ(-1.1_r, vec1.x());
	PH_EXPECT_REAL_EQ( 0.0_r, vec1.y());
	PH_EXPECT_REAL_EQ( 2.3_r, vec1.z());
	PH_EXPECT_REAL_EQ( 4.5_r, vec1.w());

	const Vector4R vec2(vec1);
	PH_EXPECT_REAL_EQ(-1.1_r, vec2.x());
	PH_EXPECT_REAL_EQ( 0.0_r, vec2.y());
	PH_EXPECT_REAL_EQ( 2.3_r, vec2.z());
	PH_EXPECT_REAL_EQ( 4.5_r, vec2.w());

	const Vector4F vec3(vec1);
	EXPECT_NEAR(-1.1f, vec3.x(), 1e-6f);
	EXPECT_NEAR( 0.0f, vec3.y(), 1e-6f);
	EXPECT_NEAR( 2.3f, vec3.z(), 1e-6f);
	EXPECT_NEAR( 4.5f, vec3.w(), 1e-6f);
}

TEST(TVector4Test, Accessors)
{
	Vector4R vec1(1.0_r, 2.0_r, 3.0_r, 4.0_r);
	vec1.x() = 1.1_r;
	vec1.y() = 2.1_r;
	vec1.z() = 3.1_r;
	vec1.w() = 4.1_r;
	PH_EXPECT_REAL_EQ(1.1_r, vec1.x());
	PH_EXPECT_REAL_EQ(2.1_r, vec1.y());
	PH_EXPECT_REAL_EQ(3.1_r, vec1.z());
	PH_EXPECT_REAL_EQ(4.1_r, vec1.w());

	vec1.r() = 1.2_r;
	vec1.g() = 2.2_r;
	vec1.b() = 3.2_r;
	vec1.a() = 4.2_r;
	PH_EXPECT_REAL_EQ(1.2_r, vec1.x());
	PH_EXPECT_REAL_EQ(2.2_r, vec1.y());
	PH_EXPECT_REAL_EQ(3.2_r, vec1.z());
	PH_EXPECT_REAL_EQ(4.2_r, vec1.w());
}

TEST(TVector4Test, Arithmetics)
{
	const Vector4R vecA(1.0_r, 2.0_r, 3.0_r, 4.0_r);
	const Vector4R vecB(5.0_r, 6.0_r, 7.0_r, 8.0_r);

	const Vector4R added = vecA.add(vecB);
	PH_EXPECT_REAL_EQ(6.0_r, added.x());
	PH_EXPECT_REAL_EQ(8.0_r, added.y());
	PH_EXPECT_REAL_EQ(10.0_r, added.z());
	PH_EXPECT_REAL_EQ(12.0_r, added.w());

	const Vector4R subtracted = vecB.sub(vecA);
	PH_EXPECT_REAL_EQ(4.0_r, subtracted.x());
	PH_EXPECT_REAL_EQ(4.0_r, subtracted.y());
	PH_EXPECT_REAL_EQ(4.0_r, subtracted.z());
	PH_EXPECT_REAL_EQ(4.0_r, subtracted.w());

	const Vector4R multiplied = vecA.mul(2.0_r);
	PH_EXPECT_REAL_EQ(2.0_r, multiplied.x());
	PH_EXPECT_REAL_EQ(4.0_r, multiplied.y());
	PH_EXPECT_REAL_EQ(6.0_r, multiplied.z());
	PH_EXPECT_REAL_EQ(8.0_r, multiplied.w());

	const Vector4R divided = vecB.div(2.0_r);
	PH_EXPECT_REAL_EQ(2.5_r, divided.x());
	PH_EXPECT_REAL_EQ(3.0_r, divided.y());
	PH_EXPECT_REAL_EQ(3.5_r, divided.z());
	PH_EXPECT_REAL_EQ(4.0_r, divided.w());
}

TEST(TVector4Test, VectorOps)
{
	const Vector4R vecA(1.0_r, 2.0_r, 3.0_r, 4.0_r);
	const Vector4R vecB(2.0_r, 3.0_r, 4.0_r, 5.0_r);

	// dot = 1*2 + 2*3 + 3*4 + 4*5 = 2 + 6 + 12 + 20 = 40
	PH_EXPECT_REAL_EQ(40.0_r, vecA.dot(vecB));

	// lengthSquared = 1^2 + 2^2 + 3^2 + 4^2 = 1 + 4 + 9 + 16 = 30
	PH_EXPECT_REAL_EQ(30.0_r, vecA.lengthSquared());
	PH_EXPECT_REAL_EQ(std::sqrt(30.0_r), vecA.length());

	const Vector4R normalized = vecA.normalize();
	PH_EXPECT_REAL_EQ(1.0_r, normalized.length());
	PH_EXPECT_REAL_EQ(1.0_r / std::sqrt(30.0_r), normalized.x());
}

TEST(TVector4Test, Comparison)
{
	const Vector4R vecA(1.0_r, 2.0_r, 3.0_r, 4.0_r);
	const Vector4R vecB(1.0_r, 2.0_r, 3.0_r, 4.0_r);
	const Vector4R vecC(1.1_r, 2.0_r, 3.0_r, 4.0_r);

	EXPECT_TRUE(vecA.isEqual(vecB));
	EXPECT_FALSE(vecA.isEqual(vecC));

	EXPECT_TRUE(vecA.isNear(vecC, 0.15_r));
	EXPECT_FALSE(vecA.isNear(vecC, 0.05_r));
}
