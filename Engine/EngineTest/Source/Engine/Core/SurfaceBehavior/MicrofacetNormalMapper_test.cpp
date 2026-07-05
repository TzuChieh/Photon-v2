#include "engine_test_constants.h"

#include <Engine/Core/SurfaceBehavior/SurfaceOptics/MicrofacetNormalMapper.h>
#include <Engine/Math/TVector3.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(MicrofacetNormalMapperTest, DirectXDiffersFromOpenGLByYAxisSign)
{
	const Vector3R encodedNormal(0.75_r, 0.25_r, 1.0_r);

	const Vector3R openGLNormal = MicrofacetNormalMapper::decodeNormalMap(
		encodedNormal,
		ENormalMapFormat::PXPYPZ_8Bits);
	const Vector3R directXNormal = MicrofacetNormalMapper::decodeNormalMap(
		encodedNormal,
		ENormalMapFormat::PXNYPZ_8Bits);

	// Normal-map Y becomes Photon local X for our Y-up convention
	EXPECT_NEAR(openGLNormal.x(), -directXNormal.x(), TEST_REAL_EPSILON);// diff by sign is expected
	EXPECT_NEAR(openGLNormal.y(),  directXNormal.y(), TEST_REAL_EPSILON);
	EXPECT_NEAR(openGLNormal.z(),  directXNormal.z(), TEST_REAL_EPSILON);
}