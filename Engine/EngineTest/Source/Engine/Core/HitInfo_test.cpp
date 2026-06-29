#include <Engine/Core/HitInfo.h>
#include <Engine/Core/Transform/StaticAffineTransform.h>
#include <Engine/Core/Quantity/Time.h>
#include <Engine/Math/TVector3.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(HitInfoTest, NoShadingNormal)
{
	HitInfo info;
	info.setAttributes(Vector3R(1, 2, 3), Vector3R(0, 1, 0));
	info.setDerivatives(
		Vector3R(1, 0, 0),
		Vector3R(0, 0, 1),
		Vector3R(0, 0, 1),
		Vector3R(1, 0, 0));

	EXPECT_FALSE(info.hasShadingNormal());
	EXPECT_FALSE(info.hasShadingTangent());

	info.computeBases();

	EXPECT_TRUE(info.getShadingBasis().getXAxis().isEqual(info.getGeometryBasis().getXAxis()));
	EXPECT_TRUE(info.getShadingBasis().getYAxis().isEqual(info.getGeometryBasis().getYAxis()));
	EXPECT_TRUE(info.getShadingBasis().getZAxis().isEqual(info.getGeometryBasis().getZAxis()));
	EXPECT_EQ(info.getShadingNormal(), info.getGeometryNormal());
}

TEST(HitInfoTest, WithShadingNormal)
{
	HitInfo info;
	info.setAttributes(
		Vector3R(1, 2, 3),
		Vector3R(0, 1, 0),
		Vector3R(0, 0, 1));
	info.setDerivatives(
		Vector3R(1, 0, 0),
		Vector3R(0, 0, 1),
		Vector3R(1, 0, 0),
		Vector3R(0, 1, 0));

	EXPECT_TRUE(info.hasShadingNormal());
	EXPECT_FALSE(info.hasShadingTangent());

	info.computeBases();

	EXPECT_EQ(info.getShadingBasis().getYAxis(), Vector3R(0, 0, 1));
}

TEST(HitInfoTest, WithShadingTangent)
{
	HitInfo info;
	info.setAttributes(
		Vector3R(1, 2, 3),
		Vector3R(0, 1, 0),
		Vector3R(0, 0, 1),
		Vector3R(1, 0, 0));
	info.setDerivatives(
		Vector3R(1, 0, 0),
		Vector3R(0, 0, 1),
		Vector3R(0, 0, 0),
		Vector3R(0, 0, 0));

	EXPECT_TRUE(info.hasShadingNormal());
	EXPECT_TRUE(info.hasShadingTangent());

	info.computeBases();

	EXPECT_EQ(info.getShadingBasis().getYAxis(), Vector3R(0, 0, 1));
	EXPECT_EQ(info.getShadingTangent(), Vector3R(1, 0, 0));
}

TEST(HitInfoTest, TransformsShadingFlags)
{
	const Transform& transform = StaticAffineTransform::makeIdentity();

	{
		HitInfo info;
		info.setAttributes(Vector3R(1, 2, 3), Vector3R(0, 1, 0));

		HitInfo transformed;
		transform.transform(info, Time{}, &transformed);

		EXPECT_FALSE(transformed.hasShadingNormal());
		EXPECT_FALSE(transformed.hasShadingTangent());
	}

	{
		HitInfo info;
		info.setAttributes(
			Vector3R(1, 2, 3),
			Vector3R(0, 1, 0),
			Vector3R(0, 0, 1));

		HitInfo transformed;
		transform.transform(info, Time{}, &transformed);

		EXPECT_TRUE(transformed.hasShadingNormal());
		EXPECT_FALSE(transformed.hasShadingTangent());
		EXPECT_EQ(transformed.getShadingNormal(), Vector3R(0, 0, 1));
	}

	{
		HitInfo info;
		info.setAttributes(
			Vector3R(1, 2, 3),
			Vector3R(0, 1, 0),
			Vector3R(0, 0, 1),
			Vector3R(1, 0, 0));

		HitInfo transformed;
		transform.transform(info, Time{}, &transformed);

		EXPECT_TRUE(transformed.hasShadingNormal());
		EXPECT_TRUE(transformed.hasShadingTangent());
		EXPECT_EQ(transformed.getShadingNormal(), Vector3R(0, 0, 1));
		EXPECT_EQ(transformed.getShadingTangent(), Vector3R(1, 0, 0));
	}
}
