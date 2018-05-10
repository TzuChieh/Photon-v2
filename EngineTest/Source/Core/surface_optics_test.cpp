#include <Core/SurfaceBehavior/BsdfHelper.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

TEST(SurfaceOpticsTest, CalculatesHalfVector)
{
	typedef ph::TVector3<float> Vec3;

	Vec3 L1 = Vec3( 0.5f, 1.0f,  0.5f).normalize();
	Vec3 V1 = Vec3(-0.5f, 1.0f, -0.5f).normalize();

	Vec3 H1;
	EXPECT_TRUE(ph::BsdfHelper::makeHalfVector(L1, V1, &H1));
	EXPECT_FLOAT_EQ(H1.length(), 1.0f);
	EXPECT_FLOAT_EQ(H1.x, 0.0f);
	EXPECT_FLOAT_EQ(H1.y, 1.0f);
	EXPECT_FLOAT_EQ(H1.z, 0.0f);

	Vec3 L2 = Vec3(0.0f, -1.0f, 0.0f).normalize();
	Vec3 V2 = Vec3(0.0f, -1.0f, 0.0f).normalize();

	Vec3 H2;
	EXPECT_TRUE(ph::BsdfHelper::makeHalfVector(L2, V2, &H2));
	EXPECT_FLOAT_EQ(H2.length(), 1.0f);
	EXPECT_FLOAT_EQ(H2.x,  0.0f);
	EXPECT_FLOAT_EQ(H2.y, -1.0f);
	EXPECT_FLOAT_EQ(H2.z,  0.0f);

	Vec3 L3 = Vec3( 1.0f, 0.0f, 0.0f).normalize();
	Vec3 V3 = Vec3(-1.0f, 0.0f, 0.0f).normalize();

	Vec3 H3;
	EXPECT_FALSE(ph::BsdfHelper::makeHalfVector(L3, V3, &H3));
}