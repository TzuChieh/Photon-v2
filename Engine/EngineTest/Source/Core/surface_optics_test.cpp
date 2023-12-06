#include <Core/SurfaceBehavior/BsdfHelper.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(SurfaceOpticsTest, CalculatesHalfVector)
{
	auto L1 = Vector3F( 0.5f, 1.0f,  0.5f).normalize();
	auto V1 = Vector3F(-0.5f, 1.0f, -0.5f).normalize();

	Vector3F H1;
	EXPECT_TRUE(ph::BsdfHelper::makeHalfVector(L1, V1, &H1));
	EXPECT_FLOAT_EQ(H1.length(), 1.0f);
	EXPECT_FLOAT_EQ(H1.x(), 0.0f);
	EXPECT_FLOAT_EQ(H1.y(), 1.0f);
	EXPECT_FLOAT_EQ(H1.z(), 0.0f);

	auto L2 = Vector3F(0.0f, -1.0f, 0.0f).normalize();
	auto V2 = Vector3F(0.0f, -1.0f, 0.0f).normalize();

	Vector3F H2;
	EXPECT_TRUE(ph::BsdfHelper::makeHalfVector(L2, V2, &H2));
	EXPECT_FLOAT_EQ(H2.length(), 1.0f);
	EXPECT_FLOAT_EQ(H2.x(),  0.0f);
	EXPECT_FLOAT_EQ(H2.y(), -1.0f);
	EXPECT_FLOAT_EQ(H2.z(),  0.0f);

	auto L3 = Vector3F( 1.0f, 0.0f, 0.0f).normalize();
	auto V3 = Vector3F(-1.0f, 0.0f, 0.0f).normalize();

	Vector3F H3;
	EXPECT_FALSE(ph::BsdfHelper::makeHalfVector(L3, V3, &H3));
}
