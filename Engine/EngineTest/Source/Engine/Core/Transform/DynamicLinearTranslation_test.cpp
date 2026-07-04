#include "engine_test_constants.h"

#include <Engine/Core/Transform/DynamicLinearTranslation.h>
#include <Engine/Math/Geometry/TAABB3D.h>
#include <Engine/Math/TVector3.h>
#include <Engine/Math/TMatrix4.h>
#include <Engine/Math/TQuaternion.h>
#include <Engine/Core/Quantity/Time.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(MathOperationsDynamicLinearTranslation, GeneratesInversed)
{
	const Vector3R   translationT0(-1.0_r);
	const Vector3R   translationT1( 1.0_r);
	const Vector3R   p1(1.1_r, 2.2_r, -3.3_r);
	const Time       timeT(0, 0.7_r);
	const Transform& t1 = DynamicLinearTranslation(translationT0, translationT1);

	std::unique_ptr<Transform> t1Inversed = t1.genInversed();
	ASSERT_TRUE(t1Inversed != nullptr);

	Vector3R p1t, p1tt;
	t1.transformP(p1, timeT, &p1t);
	t1Inversed->transformP(p1t, timeT, &p1tt);
	EXPECT_NEAR(p1.x(), p1tt.x(), TEST_REAL_EPSILON);
	EXPECT_NEAR(p1.y(), p1tt.y(), TEST_REAL_EPSILON);
	EXPECT_NEAR(p1.z(), p1tt.z(), TEST_REAL_EPSILON);
}

TEST(MathOperationsDynamicLinearTranslation, TimedTransformation)
{
	const Vector3R   translationT0(-1.0_r);
	const Vector3R   translationT1( 3.0_r);
	const Vector3R   p1(1.5_r, 2.0_r, -1.5_r);
	const Time       timeT(0, 0.75_r);
	const Transform& t1 = DynamicLinearTranslation(translationT0, translationT1);

	Vector3R p1t;
	t1.transformP(p1, timeT, &p1t);
	EXPECT_NEAR(3.5_r, p1t.x(), TEST_REAL_EPSILON);
	EXPECT_NEAR(4.0_r, p1t.y(), TEST_REAL_EPSILON);
	EXPECT_NEAR(0.5_r, p1t.z(), TEST_REAL_EPSILON);
}

TEST(MathOperationsDynamicLinearTranslation, SweepAabbContainsFullStep)
{
	const DynamicLinearTranslation transform({0, 0, 0}, {10, 0, 0});
	const AABB3D localAABB({0, 0, 0}, {2, 0, 0});

	AABB3D motionAABB;
	transform.calcSweepAABB(
		localAABB,
		Time(0, 0),
		Time(0, 1),
		2,
		1.0_r,
		&motionAABB);

	EXPECT_LE(motionAABB.getMinVertex().x(), 0.0_r);
	EXPECT_GE(motionAABB.getMaxVertex().x(), 12.0_r);
}

TEST(MathOperationsDynamicLinearTranslation, StaticTranslationHasNoMotion)
{
	const DynamicLinearTranslation transform({1, 2, 3}, {1, 2, 3});

	EXPECT_FALSE(transform.hasMotion(Time(0, 0), Time(0, 1)));
}

TEST(MathOperationsDynamicLinearTranslation, SameTimeHasNoMotion)
{
	const DynamicLinearTranslation transform({0, 0, 0}, {1, 0, 0});

	EXPECT_FALSE(transform.hasMotion(Time(0, 0.5_r), Time(0, 0.5_r)));
}

TEST(MathOperationsDynamicLinearTranslation, ChangingTranslationOverTimeHasMotion)
{
	const DynamicLinearTranslation transform({0, 0, 0}, {1, 0, 0});

	EXPECT_TRUE(transform.hasMotion(Time(0, 0), Time(0, 1)));
}
