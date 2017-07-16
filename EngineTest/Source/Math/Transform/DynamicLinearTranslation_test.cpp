#include "constants_for_math_test.h"

#include <Math/Transform/DynamicLinearTranslation.h>
#include <Math/TVector3.h>
#include <Math/TMatrix4.h>
#include <Math/TQuaternion.h>
#include <Core/Quantity/Time.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(MathOperationsDynamicLinearTranslation, GeneratesInversed)
{
	const Vector3R   translationT0(-1.0_r);
	const Vector3R   translationT1( 1.0_r);
	const Vector3R   p1(1.1_r, 2.2_r, -3.3_r);
	const Time       timeT(0, 0, 0.7_r);
	const Transform& t1 = DynamicLinearTranslation(translationT0, translationT1);

	std::unique_ptr<Transform> t1Inversed = t1.genInversed();
	ASSERT_TRUE(t1Inversed != nullptr);

	Vector3R p1t, p1tt;
	t1.transformP(p1, timeT, &p1t);
	t1Inversed->transformP(p1t, timeT, &p1tt);
	EXPECT_NEAR(p1.x, p1tt.x, TEST_REAL_EPSILON);
	EXPECT_NEAR(p1.y, p1tt.y, TEST_REAL_EPSILON);
	EXPECT_NEAR(p1.z, p1tt.z, TEST_REAL_EPSILON);
}

TEST(MathOperationsDynamicLinearTranslation, TimedTransformation)
{
	const Vector3R   translationT0(-1.0_r);
	const Vector3R   translationT1( 3.0_r);
	const Vector3R   p1(1.5_r, 2.0_r, -1.5_r);
	const Time       timeT(0, 0, 0.75_r);
	const Transform& t1 = DynamicLinearTranslation(translationT0, translationT1);

	Vector3R p1t;
	t1.transformP(p1, timeT, &p1t);
	EXPECT_NEAR(3.5_r, p1t.x, TEST_REAL_EPSILON);
	EXPECT_NEAR(4.0_r, p1t.y, TEST_REAL_EPSILON);
	EXPECT_NEAR(0.5_r, p1t.z, TEST_REAL_EPSILON);
}