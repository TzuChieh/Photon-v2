#include <Math/Geometry/TSphere.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(TSphereTest, SphericalToCartesian)
{
	const float pi = 3.1415926536f;
	const TSphere<float> unitSphere(1.0f);

	// North pole
	{
		const auto northPole = unitSphere.phiThetaToSurface({0.123f, 0.0f});
		EXPECT_NEAR(northPole.x, 0.0f, 1e-6);
		EXPECT_NEAR(northPole.y, 1.0f, 1e-6);
		EXPECT_NEAR(northPole.z, 0.0f, 1e-6);
	}

	// South pole
	{
		const auto southPole = unitSphere.phiThetaToSurface({0.456f, pi});
		EXPECT_NEAR(southPole.x, 0.0f, 1e-6);
		EXPECT_NEAR(southPole.y, -1.0f, 1e-6);
		EXPECT_NEAR(southPole.z, 0.0f, 1e-6);
	}

	// +z axis
	{
		const auto pz = unitSphere.phiThetaToSurface({0.0f, pi / 2.0f});
		EXPECT_NEAR(pz.x, 0.0f, 1e-6);
		EXPECT_NEAR(pz.y, 0.0f, 1e-6);
		EXPECT_NEAR(pz.z, 1.0f, 1e-6);
	}

	// +x axis
	{
		const auto px = unitSphere.phiThetaToSurface({pi / 2.0f, pi / 2.0f});
		EXPECT_NEAR(px.x, 1.0f, 1e-6);
		EXPECT_NEAR(px.y, 0.0f, 1e-6);
		EXPECT_NEAR(px.z, 0.0f, 1e-6);
	}

	// TODO: more arbitrary angles
}
