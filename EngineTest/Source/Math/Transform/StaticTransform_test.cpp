#include "constants_for_test.h"

#include <Math/Transform/StaticAffineTransform.h>
#include <Math/TVector3.h>
#include <Math/TMatrix4.h>
#include <Math/TQuaternion.h>
#include <Math/Transform/TDecomposedTransform.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(StaticAffineTransformTest, TransformsVector3rAsPoint)
{
	// trial 1
	
	const Transform& t1 = StaticAffineTransform::IDENTITY();
	const Vector3R p1(-0.3_r, 0.0_r, 0.3_r);
	Vector3R answer1;
	t1.transformP(p1, &answer1);
	EXPECT_NEAR(p1.x, answer1.x, TEST_REAL_EPSILON);
	EXPECT_NEAR(p1.y, answer1.y, TEST_REAL_EPSILON);
	EXPECT_NEAR(p1.z, answer1.z, TEST_REAL_EPSILON);

	// trial 2

	const Transform& t2 = StaticAffineTransform(
		Matrix4R().initTranslation(-1.3_r,  1.0_r, 0.0_r),
		Matrix4R().initTranslation( 1.3_r, -1.0_r, 0.0_r));
	const Vector3R p2(0.0_r, 0.0_r, 0.0_r);
	Vector3R answer2;
	t2.transformP(p2, &answer2);
	EXPECT_NEAR(-1.3_r, answer2.x, TEST_REAL_EPSILON);
	EXPECT_NEAR( 1.0_r, answer2.y, TEST_REAL_EPSILON);
	EXPECT_NEAR( 0.0_r, answer2.z, TEST_REAL_EPSILON);
}

TEST(StaticAffineTransformTest, TransformsVector3rAsVector)
{
	// trial 1

	const Transform& t1 = StaticAffineTransform::IDENTITY();
	const Vector3R v1(-0.3_r, 0.0_r, 0.3_r);
	Vector3R answer1;
	t1.transformO(v1, &answer1);
	EXPECT_NEAR(v1.x, answer1.x, TEST_REAL_EPSILON);
	EXPECT_NEAR(v1.y, answer1.y, TEST_REAL_EPSILON);
	EXPECT_NEAR(v1.z, answer1.z, TEST_REAL_EPSILON);

	// trial 2

	const Transform& t2 = StaticAffineTransform(
		Matrix4R().initTranslation(-1.3_r,  1.0_r, 0.0_r),
		Matrix4R().initTranslation( 1.3_r, -1.0_r, 0.0_r));
	const Vector3R v2(1.0_r, 2.0_r, -3.0_r);
	Vector3R answer2;
	t2.transformO(v2, &answer2);
	EXPECT_NEAR(v2.x, answer2.x, TEST_REAL_EPSILON);
	EXPECT_NEAR(v2.y, answer2.y, TEST_REAL_EPSILON);
	EXPECT_NEAR(v2.z, answer2.z, TEST_REAL_EPSILON);

	// trial 3

	const Transform& t3 = StaticAffineTransform(
		Matrix4R().initRotation(QuaternionR(Vector3R(0, 1, 0),  3.141592653589793_r)),
		Matrix4R().initRotation(QuaternionR(Vector3R(0, 1, 0), -3.141592653589793_r)));
	const Vector3R v3(3.0_r, -2.0_r, 1.0_r);
	Vector3R answer3;
	t3.transformO(v3, &answer3);
	EXPECT_NEAR(-v3.x, answer3.x, TEST_REAL_EPSILON);
	EXPECT_NEAR( v3.y, answer3.y, TEST_REAL_EPSILON);
	EXPECT_NEAR(-v3.z, answer3.z, TEST_REAL_EPSILON);

	// trial 4

	const Transform& t4 = StaticAffineTransform(
		Matrix4R().initScale(2.0_r,         1.0_r, 1.0_r),
		Matrix4R().initScale(1.0_r / 2.0_r, 1.0_r, 1.0_r));

	const Vector3R v4(1.0_r, 1.0_r, 0.0_r);

	Vector3R answer4;
	t4.transformO(v4, &answer4);
	answer4.divLocal(answer4.x);// normalizing x component to 1
	EXPECT_NEAR(answer4.x, 1.0_r, TEST_REAL_EPSILON);
	EXPECT_NEAR(answer4.y, 2.0_r, TEST_REAL_EPSILON);
	EXPECT_NEAR(answer4.z, 0.0_r, TEST_REAL_EPSILON);
}

TEST(StaticAffineTransformTest, GeneratesInversed)
{
	// trial 1

	const Vector3R p1(1.1_r, 2.2_r, -3.3_r);
	const Vector3R v1(1.0_r, 2.0_r, -3.0_r);
	TDecomposedTransform<real> decom1;
	decom1.translate(1, 2, 3);
	decom1.rotate(Vector3R(-0.5_r, 0.1_r, 3.3_r), 50);
	decom1.scale(-2, 1, 0.1_r);
	const Transform& t1 = StaticAffineTransform::makeForward(decom1);

	std::unique_ptr<Transform> t1Inversed = t1.genInversed();
	ASSERT_TRUE(t1Inversed != nullptr);

	Vector3R p1t, p1tt;
	t1.transformP(p1, &p1t);
	t1Inversed->transformP(p1t, &p1tt);
	EXPECT_NEAR(p1.x, p1tt.x, TEST_REAL_EPSILON);
	EXPECT_NEAR(p1.y, p1tt.y, TEST_REAL_EPSILON);
	EXPECT_NEAR(p1.z, p1tt.z, TEST_REAL_EPSILON);

	Vector3R v1t, v1tt;
	t1.transformV(v1, &v1t);
	t1Inversed->transformV(v1t, &v1tt);
	EXPECT_NEAR(v1.x, v1tt.x, TEST_REAL_EPSILON);
	EXPECT_NEAR(v1.y, v1tt.y, TEST_REAL_EPSILON);
	EXPECT_NEAR(v1.z, v1tt.z, TEST_REAL_EPSILON);
}
