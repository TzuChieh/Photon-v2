#include <Math/TOrthonormalBasis3.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

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
}

TEST(TOrthonormalBasis3Test, Trigonometry)
{
	using Vec = TVector3<float>;

	TOrthonormalBasis3<float> basis;
	basis.setXAxis(Vec(1, 0, 0));
	basis.setYAxis(Vec(0, 1, 0));
	basis.setZAxis(Vec(0, 0, 1));

	Vec unitVec1(1, 0, 0);
	EXPECT_FLOAT_EQ(basis.cosTheta(unitVec1),  0);
	EXPECT_FLOAT_EQ(basis.cos2Theta(unitVec1), 0);
	EXPECT_FLOAT_EQ(basis.sinTheta(unitVec1),  1);
	EXPECT_FLOAT_EQ(basis.sin2Theta(unitVec1), 1);
	EXPECT_FLOAT_EQ(basis.cosPhi(unitVec1),    0);
	EXPECT_FLOAT_EQ(basis.cos2Phi(unitVec1),   0);
	EXPECT_FLOAT_EQ(basis.sinPhi(unitVec1),    1);
	EXPECT_FLOAT_EQ(basis.sin2Phi(unitVec1),   1);

	Vec unitVec2(0, 0, 1);
	EXPECT_FLOAT_EQ(basis.cosTheta(unitVec2),  0);
	EXPECT_FLOAT_EQ(basis.cos2Theta(unitVec2), 0);
	EXPECT_FLOAT_EQ(basis.sinTheta(unitVec2),  1);
	EXPECT_FLOAT_EQ(basis.sin2Theta(unitVec2), 1);
	EXPECT_FLOAT_EQ(basis.cosPhi(unitVec2),    1);
	EXPECT_FLOAT_EQ(basis.cos2Phi(unitVec2),   1);
	EXPECT_FLOAT_EQ(basis.sinPhi(unitVec2),    0);
	EXPECT_FLOAT_EQ(basis.sin2Phi(unitVec2),   0);
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

