#include "constants_for_test.h"

#include <Math/TVector3.h>

#include <gtest/gtest.h>

#include <type_traits>

using namespace ph;

TEST(TVector3Test, Requirements)
{
	EXPECT_TRUE(std::is_trivially_copyable_v<Vector3R>);
	EXPECT_TRUE(std::is_trivially_copyable_v<Vector3F>);
	EXPECT_TRUE(std::is_trivially_copyable_v<Vector3D>);
	EXPECT_TRUE(std::is_trivially_copyable_v<Vector3S>);
}

TEST(TVector3Test, Constructs)
{
	const Vector3R vec1(-1.1_r, 0.0_r, 2.3_r);
	EXPECT_EQ(-1.1_r, vec1.x);
	EXPECT_EQ( 0.0_r, vec1.y);
	EXPECT_EQ( 2.3_r, vec1.z);
}

TEST(TVector3Test, AddsTwoVectors)
{
	const Vector3R vecA1(0.0_r, 1.0_r, -1.0_r), vecB1(0.0_r, -1.0_r, 1.0_r);
	const Vector3R answer1 = vecA1.add(vecB1);
	EXPECT_NEAR(0.0_r, answer1.x, TEST_REAL_EPSILON);
	EXPECT_NEAR(0.0_r, answer1.y, TEST_REAL_EPSILON);
	EXPECT_NEAR(0.0_r, answer1.z, TEST_REAL_EPSILON);

	const Vector3R vecA2(0.0_r, 10.0_r, 0.1_r), vecB2(10.5_r, 25.3_r, -4.8_r);
	const Vector3R answer2 = vecA2.add(vecB2);
	EXPECT_NEAR(10.5_r, answer2.x, TEST_REAL_EPSILON);
	EXPECT_NEAR(35.3_r, answer2.y, TEST_REAL_EPSILON);
	EXPECT_NEAR(-4.7_r, answer2.z, TEST_REAL_EPSILON);
}

TEST(TVector3Test, SubtractsTwoVectors)
{
	const Vector3R vecA1(1.0_r, 1.0_r, -1.0_r), vecB1(0.0_r, -1.0_r, 1.0_r);
	const Vector3R answer1 = vecA1.sub(vecB1);
	EXPECT_NEAR( 1.0_r, answer1.x, TEST_REAL_EPSILON);
	EXPECT_NEAR( 2.0_r, answer1.y, TEST_REAL_EPSILON);
	EXPECT_NEAR(-2.0_r, answer1.z, TEST_REAL_EPSILON);

	const Vector3R vecA2(0.0_r, 2.3_r, -4.5_r), vecB2(0.0_r, -0.1_r, -4.5_r);
	const Vector3R answer2 = vecA2.sub(vecB2);
	EXPECT_NEAR(0.0_r, answer2.x, TEST_REAL_EPSILON);
	EXPECT_NEAR(2.4_r, answer2.y, TEST_REAL_EPSILON);
	EXPECT_NEAR(0.0_r, answer2.z, TEST_REAL_EPSILON);
}

TEST(TVector3Test, ComparesEquality)
{
	const Vector3R vecA1(0.0_r, -1.0_r, 2.3_r), vecB1(0.0_r, -1.0_r, 2.3_r);
	EXPECT_TRUE(vecA1.equals(vecB1));

	const Vector3R vecA2(0.15_r, -0.05_r, 0.0_r), vecB2(0.1_r, 0.03_r, 0.0_r);
	EXPECT_TRUE(vecA2.equals(vecB2, 0.15_r));

	const Vector3R vecA3(0.0_r, -0.05_r, -0.1_r), vecB3(0.0_r, 0.03_r, 0.1_r);
	EXPECT_FALSE(vecA3.equals(vecB3, 0.15_r));
}

TEST(TVector3Test, WeightedSum)
{
	const Vector3R vec1(1, 0, 0);
	const Vector3R vec2(0, 1, 0);
	const Vector3R vec3(0, 0, 1);

	const Vector3R result1 = Vector3R::weightedSum(vec1, 1, vec2, 1, vec3, 1);
	EXPECT_TRUE(result1.x == 1 && result1.y == 1 && result1.z == 1);

	const Vector3R result2 = Vector3R::weightedSum(vec1, 1, vec1, 0, vec1, 0);
	EXPECT_TRUE(result2.x == 1 && result2.y == 0 && result2.z == 0);
}