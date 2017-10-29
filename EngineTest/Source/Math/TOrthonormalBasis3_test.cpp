#include <Math/TOrthonormalBasis3.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(TOrthonormalBasis3Test, Renormalize)
{
	TOrthonormalBasis3<float> basis;
	basis.xAxis = TVector3<float>(0, 100, 0);
	basis.yAxis = TVector3<float>(-200, 0, 0);
	basis.zAxis = TVector3<float>(0, 0, 10);
	basis.renormalize();

	EXPECT_FLOAT_EQ(basis.xAxis.length(), 1.0f);
	EXPECT_FLOAT_EQ(basis.yAxis.length(), 1.0f);
	EXPECT_FLOAT_EQ(basis.zAxis.length(), 1.0f);
}

TEST(TOrthonormalBasis3Test, VectorTransformation)
{
	TOrthonormalBasis3<float> basis;
	basis.xAxis = TVector3<float>(0, 100, 0);
	basis.yAxis = TVector3<float>(-200, 0, 0);
	basis.zAxis = TVector3<float>(0, 0, 10);
	basis.renormalize();

	TVector3<float> worldVec(1, 2, 3);
	TVector3<float> localVec = basis.worldToLocal(worldVec);

	EXPECT_FLOAT_EQ(localVec.x, 2);
	EXPECT_FLOAT_EQ(localVec.y, -1);
	EXPECT_FLOAT_EQ(localVec.z, 3);
}

TEST(TOrthonormalBasis3Test, TrigonometryCornerCase)
{
	typedef TVector3<float> Vec;

	TOrthonormalBasis3<float> basis;
	basis.xAxis = Vec(1, 0, 0);
	basis.yAxis = Vec(0, 1, 0);
	basis.zAxis = Vec(0, 0, 1);

	Vec unitVec1(0, 1, 0);
	EXPECT_TRUE(basis.cosTheta(unitVec1) == 1);
	EXPECT_TRUE(basis.cosPhi(unitVec1) == 1);
}