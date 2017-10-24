#include <Math/TOrthonormalBasis3.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

TEST(TOrthonormalBasis3Test, Renormalize)
{
	ph::TOrthonormalBasis3<float> basis;
	basis.xAxis = ph::TVector3<float>(0, 100, 0);
	basis.yAxis = ph::TVector3<float>(-200, 0, 0);
	basis.zAxis = ph::TVector3<float>(0, 0, 10);
	basis.renormalize();

	EXPECT_FLOAT_EQ(basis.xAxis.length(), 1.0f);
	EXPECT_FLOAT_EQ(basis.yAxis.length(), 1.0f);
	EXPECT_FLOAT_EQ(basis.zAxis.length(), 1.0f);
}

TEST(TOrthonormalBasis3Test, VectorTransformation)
{
	ph::TOrthonormalBasis3<float> basis;
	basis.xAxis = ph::TVector3<float>(0, 100, 0);
	basis.yAxis = ph::TVector3<float>(-200, 0, 0);
	basis.zAxis = ph::TVector3<float>(0, 0, 10);
	basis.renormalize();

	ph::TVector3<float> worldVec(1, 2, 3);
	ph::TVector3<float> localVec = basis.worldToLocal(worldVec);

	EXPECT_FLOAT_EQ(localVec.x, 2);
	EXPECT_FLOAT_EQ(localVec.y, -1);
	EXPECT_FLOAT_EQ(localVec.z, 3);
}