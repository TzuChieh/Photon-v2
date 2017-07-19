#include "constants_for_test.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(MathOperationsVector2R, Constructs)
{
	// trial 1: defaults to (0, 0)

	const Vector2R vec1;
	EXPECT_EQ(vec1.x, 0.0_r);
	EXPECT_EQ(vec1.y, 0.0_r);

	// trial 2: element-by-element construct

	const Vector2R vec2(-1.1_r, 2.2_r);
	EXPECT_EQ(vec2.x, -1.1_r);
	EXPECT_EQ(vec2.y,  2.2_r);

	// trial 3: fill with scalar

	const Vector2R vec3(5.0_r);
	EXPECT_EQ(vec3.x, 5.0_r);
	EXPECT_EQ(vec3.y, 5.0_r);

	// trial 4: copying

	const Vector2R vec4a(1.0_r, 2.0_r);
	const Vector2R vec4b(vec4a);
	EXPECT_EQ(vec4a.x, vec4b.x);
	EXPECT_EQ(vec4a.y, vec4b.y);

	// trial 5: assigning

	Vector2R vec5a;
	const Vector2R vec5b(1.0_r, 2.0_r);
	vec5a = vec5b;
	EXPECT_EQ(vec5a.x, vec5b.x);
	EXPECT_EQ(vec5a.y, vec5b.y);

	// trial 6: casting

	Vector2R vec6r;
	const TVector2<int32> vec6i(-3, 3);
	vec6r = Vector2R(vec6i);
	EXPECT_NEAR(vec6r.x, -3.0_r, TEST_REAL_EPSILON);
	EXPECT_NEAR(vec6r.y,  3.0_r, TEST_REAL_EPSILON);
}

TEST(MathOperationsVector2R, AddTwoVectors)
{
	const Vector2R vecA(-1.5_r, 2.0_r);
	const Vector2R vecB( 1.0_r, 3.5_r);
	const real answerX = -0.5_r;
	const real answerY =  5.5_r;

	// trial 1: copied adding

	const Vector2R result1 = vecA.add(vecB);
	EXPECT_NEAR(result1.x, answerX, TEST_REAL_EPSILON);
	EXPECT_NEAR(result1.y, answerY, TEST_REAL_EPSILON);

	// trial 2: localized adding

	const Vector2R result2 = Vector2R(vecA).addLocal(vecB);
	EXPECT_NEAR(result2.x, answerX, TEST_REAL_EPSILON);
	EXPECT_NEAR(result2.y, answerY, TEST_REAL_EPSILON);
}

TEST(MathOperationsVector2R, SubTwoVectors)
{
	const Vector2R vecA(-1.5_r, 2.0_r);
	const Vector2R vecB( 1.0_r, 3.5_r);
	const real answerX = -2.5_r;
	const real answerY = -1.5_r;

	// trial 1: copied subtracting

	const Vector2R result1 = vecA.sub(vecB);
	EXPECT_NEAR(result1.x, answerX, TEST_REAL_EPSILON);
	EXPECT_NEAR(result1.y, answerY, TEST_REAL_EPSILON);

	// trial 2: localized subtracting

	const Vector2R result2 = Vector2R(vecA).subLocal(vecB);
	EXPECT_NEAR(result2.x, answerX, TEST_REAL_EPSILON);
	EXPECT_NEAR(result2.y, answerY, TEST_REAL_EPSILON);
}

TEST(MathOperationsVector2R, MulTwoVectors)
{
	const Vector2R vecA(-1.5_r, 2.0_r);
	const Vector2R vecB( 2.0_r, 1.5_r);
	const real answerX = -3.0_r;
	const real answerY =  3.0_r;

	// trial 1: copied multiplying

	const Vector2R result1 = vecA.mul(vecB);
	EXPECT_NEAR(result1.x, answerX, TEST_REAL_EPSILON);
	EXPECT_NEAR(result1.y, answerY, TEST_REAL_EPSILON);

	// trial 2: localized multiplying

	const Vector2R result2 = Vector2R(vecA).mulLocal(vecB);
	EXPECT_NEAR(result2.x, answerX, TEST_REAL_EPSILON);
	EXPECT_NEAR(result2.y, answerY, TEST_REAL_EPSILON);
}

TEST(MathOperationsVector2R, MulVectorWithScalar)
{
	const Vector2R vec(-1.5_r, 2.0_r);
	const real scalar = 2.0_r;
	const real answerX = -3.0_r;
	const real answerY =  4.0_r;

	// trial 1: copied multiplying

	const Vector2R result1 = vec.mul(scalar);
	EXPECT_NEAR(result1.x, answerX, TEST_REAL_EPSILON);
	EXPECT_NEAR(result1.y, answerY, TEST_REAL_EPSILON);

	// trial 2: localized multiplying

	const Vector2R result2 = Vector2R(vec).mulLocal(scalar);
	EXPECT_NEAR(result2.x, answerX, TEST_REAL_EPSILON);
	EXPECT_NEAR(result2.y, answerY, TEST_REAL_EPSILON);
}

TEST(MathOperationsVector2R, DivTwoVectors)
{
	const Vector2R vecA(-1.0_r, 2.0_r);
	const Vector2R vecB( 2.0_r, 4.0_r);
	const real answerX = -0.5_r;
	const real answerY =  0.5_r;

	// trial 1: copied dividing

	const Vector2R result1 = vecA.div(vecB);
	EXPECT_NEAR(result1.x, answerX, TEST_REAL_EPSILON);
	EXPECT_NEAR(result1.y, answerY, TEST_REAL_EPSILON);

	// trial 2: localized dividing

	const Vector2R result2 = Vector2R(vecA).divLocal(vecB);
	EXPECT_NEAR(result2.x, answerX, TEST_REAL_EPSILON);
	EXPECT_NEAR(result2.y, answerY, TEST_REAL_EPSILON);
}

TEST(MathOperationsVector2R, DivVectorWithScalar)
{
	const Vector2R vec(-1.2_r, 2.0_r);
	const real scalar = 4.0_r;
	const real answerX = -0.3_r;
	const real answerY =  0.5_r;

	// trial 1: copied dividing

	const Vector2R result1 = vec.div(scalar);
	EXPECT_NEAR(result1.x, answerX, TEST_REAL_EPSILON);
	EXPECT_NEAR(result1.y, answerY, TEST_REAL_EPSILON);

	// trial 2: localized dividing

	const Vector2R result2 = Vector2R(vec).divLocal(scalar);
	EXPECT_NEAR(result2.x, answerX, TEST_REAL_EPSILON);
	EXPECT_NEAR(result2.y, answerY, TEST_REAL_EPSILON);
}

TEST(MathOperationsVector2R, ComparesEquality)
{
	// trial 1: strict equality
	
	const Vector2R vec1a(-1.5_r, 1.5_r);
	const Vector2R vec1b(-1.5_r, 1.5_r);
	EXPECT_TRUE(vec1a.equals(vec1b));

	// trial 2: marginal equality

	const Vector2R vec2a(-1.5_r, 1.5_r);
	const Vector2R vec2b(-1.6_r, 1.3_r);
	EXPECT_TRUE(vec2a.equals(vec2b, 0.3_r));
}