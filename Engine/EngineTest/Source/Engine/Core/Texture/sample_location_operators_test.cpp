#include "engine_test_constants.h"

#include <Engine/Core/Texture/Function/sample_location_operators.h>
#include <Engine/Core/Texture/SampleLocation.h>
#include <Engine/Math/Color/color_enums.h>
#include <Engine/Math/math.h>
#include <Engine/Math/TMatrix4.h>
#include <Engine/Math/TVector3.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(SampleLocationOperatorTest, AffineTransformRotatesUvw)
{
	Matrix4R matrix;
	matrix.initRotation({0.0_r, 0.0_r, 1.0_r}, math::to_radians(90.0_r));
	const texfunc::AffineUvwTransform transform(matrix);
	const SampleLocation mappedLocation = transform(
		SampleLocation({0.25_r, 0.0_r, 0.0_r}, EColorUsage::Raw));

	EXPECT_NEAR(0.0_r, mappedLocation.uvw().x(), TEST_REAL_EPSILON);
	EXPECT_NEAR(0.25_r, mappedLocation.uvw().y(), TEST_REAL_EPSILON);
	EXPECT_NEAR(0.0_r, mappedLocation.uvw().z(), TEST_REAL_EPSILON);
}

TEST(SampleLocationOperatorTest, AffineTransformTranslatesUvw)
{
	Matrix4R matrix;
	matrix.initTranslation(1.0_r, -2.0_r, 3.0_r);

	const texfunc::AffineUvwTransform transform(matrix);
	const SampleLocation mappedLocation = transform(
		SampleLocation({0.25_r, 0.5_r, 0.75_r}, EColorUsage::Raw));

	EXPECT_EQ(mappedLocation.uvw(), Vector3R(1.25_r, -1.5_r, 3.75_r));
}
