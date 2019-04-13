#include <Math/TOrthonormalBasis3.h>
#include <Math/TVector3.h>
#include <Math/math.h>

#include <gtest/gtest.h>

#include <cmath>

using namespace ph;

TEST(TOrthonormalBasis3Test, Renormalize)
{
	TOrthonormalBasis3<float> basis;
	basis.setXAxis(TVector3<float>(0, 100, 0));
	basis.setYAxis(TVector3<float>(-200, 0, 0));
	basis.setZAxis(TVector3<float>(0, 0, 10));
	basis.renormalize();

	EXPECT_FLOAT_EQ(basis.getXAxis().length(), 1.0f);
	EXPECT_FLOAT_EQ(basis.getYAxis().length(), 1.0f);
	EXPECT_FLOAT_EQ(basis.getZAxis().length(), 1.0f);
}

TEST(TOrthonormalBasis3Test, VectorTransformation)
{
	TOrthonormalBasis3<float> basis;
	basis.setXAxis(TVector3<float>(0, 100, 0));
	basis.setYAxis(TVector3<float>(-200, 0, 0));
	basis.setZAxis(TVector3<float>(0, 0, 10));
	basis.renormalize();

	TVector3<float> worldVec(1, 2, 3);
	TVector3<float> localVec = basis.worldToLocal(worldVec);
	EXPECT_FLOAT_EQ(localVec.x, 2);
	EXPECT_FLOAT_EQ(localVec.y, -1);
	EXPECT_FLOAT_EQ(localVec.z, 3);

	worldVec = basis.localToWorld(localVec);
	EXPECT_FLOAT_EQ(worldVec.x, 1);
	EXPECT_FLOAT_EQ(worldVec.y, 2);
	EXPECT_FLOAT_EQ(worldVec.z, 3);
}

TEST(TOrthonormalBasis3Test, Trigonometry)
{
	using Vec = TVector3<float>;

	constexpr float MAX_ABS_ERROR = 1e-5f;

	TOrthonormalBasis3<float> axisAlignedBasis;
	axisAlignedBasis.setXAxis(Vec(1, 0, 0));
	axisAlignedBasis.setYAxis(Vec(0, 1, 0));
	axisAlignedBasis.setZAxis(Vec(0, 0, 1));

	// test for trivial axis-aligned cases

	Vec unitVec1(1, 0, 0);
	EXPECT_NEAR(axisAlignedBasis.cosTheta(unitVec1),  0, MAX_ABS_ERROR);
	EXPECT_NEAR(axisAlignedBasis.cos2Theta(unitVec1), 0, MAX_ABS_ERROR);
	EXPECT_NEAR(axisAlignedBasis.sinTheta(unitVec1),  1, MAX_ABS_ERROR);
	EXPECT_NEAR(axisAlignedBasis.sin2Theta(unitVec1), 1, MAX_ABS_ERROR);
	EXPECT_NEAR(axisAlignedBasis.cosPhi(unitVec1),    0, MAX_ABS_ERROR);
	EXPECT_NEAR(axisAlignedBasis.cos2Phi(unitVec1),   0, MAX_ABS_ERROR);
	EXPECT_NEAR(axisAlignedBasis.sinPhi(unitVec1),    1, MAX_ABS_ERROR);
	EXPECT_NEAR(axisAlignedBasis.sin2Phi(unitVec1),   1, MAX_ABS_ERROR);

	Vec unitVec2(0, 0, 1);
	EXPECT_NEAR(axisAlignedBasis.cosTheta(unitVec2),  0, MAX_ABS_ERROR);
	EXPECT_NEAR(axisAlignedBasis.cos2Theta(unitVec2), 0, MAX_ABS_ERROR);
	EXPECT_NEAR(axisAlignedBasis.sinTheta(unitVec2),  1, MAX_ABS_ERROR);
	EXPECT_NEAR(axisAlignedBasis.sin2Theta(unitVec2), 1, MAX_ABS_ERROR);
	EXPECT_NEAR(axisAlignedBasis.cosPhi(unitVec2),    1, MAX_ABS_ERROR);
	EXPECT_NEAR(axisAlignedBasis.cos2Phi(unitVec2),   1, MAX_ABS_ERROR);
	EXPECT_NEAR(axisAlignedBasis.sinPhi(unitVec2),    0, MAX_ABS_ERROR);
	EXPECT_NEAR(axisAlignedBasis.sin2Phi(unitVec2),   0, MAX_ABS_ERROR);

	// test for UV divided locations on unit sphere (axis-aligned)

	constexpr float DELTA_DEGREES_THETA = 10.0f;
	constexpr float DELTA_DEGREES_PHI   = 10.0f;
	for(float thetaDeg = DELTA_DEGREES_THETA / 2.0f; thetaDeg < 180.0f; thetaDeg += DELTA_DEGREES_THETA)
	{
		for(float phiDeg = DELTA_DEGREES_PHI / 2.0f; phiDeg < 360.0f; phiDeg += DELTA_DEGREES_PHI)
		{
			const float theta = math::to_radians(thetaDeg);
			const float phi   = math::to_radians(phiDeg);

			const Vec unitUvDir(
				std::sin(theta) * std::sin(phi),
				std::cos(theta),
				std::sin(theta) * std::cos(phi));

			EXPECT_NEAR(
				axisAlignedBasis.cosTheta(unitUvDir), std::cos(theta), 
				MAX_ABS_ERROR);

			EXPECT_NEAR(
				axisAlignedBasis.cos2Theta(unitUvDir), std::cos(theta) * std::cos(theta),
				MAX_ABS_ERROR);

			EXPECT_NEAR(
				axisAlignedBasis.sinTheta(unitUvDir), std::sin(theta), 
				MAX_ABS_ERROR);

			EXPECT_NEAR(
				axisAlignedBasis.sin2Theta(unitUvDir), std::sin(theta) * std::sin(theta), 
				MAX_ABS_ERROR);

			EXPECT_NEAR(
				axisAlignedBasis.cosPhi(unitUvDir), std::cos(phi), 
				MAX_ABS_ERROR);

			EXPECT_NEAR(
				axisAlignedBasis.cos2Phi(unitUvDir), std::cos(phi) * std::cos(phi),
				MAX_ABS_ERROR);

			EXPECT_NEAR(
				axisAlignedBasis.sinPhi(unitUvDir), std::sin(phi),
				MAX_ABS_ERROR);

			EXPECT_NEAR(
				axisAlignedBasis.sin2Phi(unitUvDir), std::sin(phi) * std::sin(phi),
				MAX_ABS_ERROR);
		}
	}

	// TODO: non axis-aligned UV divided locations
}

TEST(TOrthonormalBasis3Test, TrigonometryCornerCase)
{
	typedef TVector3<float> Vec;

	TOrthonormalBasis3<float> basis;
	basis.setXAxis(Vec(1, 0, 0));
	basis.setYAxis(Vec(0, 1, 0));
	basis.setZAxis(Vec(0, 0, 1));

	Vec unitVec1(0, 1, 0);
	EXPECT_TRUE(basis.cosTheta(unitVec1) == 1);
	EXPECT_TRUE(basis.cosPhi(unitVec1) == 1);
}

