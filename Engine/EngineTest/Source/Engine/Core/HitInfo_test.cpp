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

TEST(HitInfoTest, ComputesShadingBasisFromTangent)
{
	const Vector3R position(1, 2, 3);
	const Vector3R normal(0, 0, 1);
	const Vector3R tangent(1, 0, 0);
	const Vector3R bitangent(0, 1, 0);

	// Tangent-only input derives the bitangent
	{
		HitInfo info;
		info.setAttributes(position, normal, normal, tangent);
		info.computeBases();

		EXPECT_TRUE(info.hasShadingNormal());
		EXPECT_TRUE(info.hasShadingTangent());
		EXPECT_EQ(info.getShadingBasis().getXAxis(), bitangent);
		EXPECT_EQ(info.getShadingBasis().getYAxis(), normal);
		EXPECT_EQ(info.getShadingBasis().getZAxis(), tangent);
	}

	// An explicit bitangent controls handedness
	{
		HitInfo info;
		info.setAttributes(position, normal, normal, tangent, -bitangent);
		info.computeBases();

		EXPECT_EQ(info.getShadingBasis().getXAxis(), -bitangent);
		EXPECT_EQ(info.getShadingBasis().getYAxis(), normal);
		EXPECT_EQ(info.getShadingBasis().getZAxis(), tangent);
	}

	// A bitangent is used if the tangent is degenerate
	{
		HitInfo info;
		info.setAttributes(position, normal, normal, normal, bitangent);
		info.computeBases();

		EXPECT_EQ(info.getShadingBasis().getXAxis(), bitangent);
		EXPECT_EQ(info.getShadingBasis().getYAxis(), normal);
		EXPECT_EQ(info.getShadingBasis().getZAxis(), tangent);
	}
}

TEST(HitInfoTest, ReflectedTransformFlipsTangentHandedness)
{
	HitInfo info;
	info.setAttributes(
		Vector3R(1, 2, 3),
		Vector3R(0, 1, 0),
		Vector3R(0, 1, 0),
		Vector3R(0, 0, 1),
		Vector3R(-1, 0, 0));

	TDecomposedTransform<real> transformData;
	transformData.scale(-1, 1, 1);
	const StaticAffineTransform transform = StaticAffineTransform::makeForward(transformData);

	HitInfo transformed;
	transform.transform(info, Time{}, &transformed);
	transformed.computeBases();

	EXPECT_EQ(transformed.getShadingBasis().getXAxis(), Vector3R(1, 0, 0));
	EXPECT_EQ(transformed.getShadingBasis().getZAxis(), Vector3R(0, 0, 1));
}

TEST(HitInfoTest, TransformsShadingFlags)
{
	const Transform& transform = StaticAffineTransform::IDENTITY();

	// Geometry normal only
	{
		HitInfo info;
		info.setAttributes(Vector3R(1, 2, 3), Vector3R(0, 1, 0));

		HitInfo transformed;
		transform.transform(info, Time{}, &transformed);

		EXPECT_FALSE(transformed.hasShadingNormal());
		EXPECT_FALSE(transformed.hasShadingTangent());
	}

	// Explicit shading normal
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

	// Explicit shading tangent
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
		EXPECT_EQ(transformed.getShadingBitangent(), Vector3R(0, 1, 0));
	}
}
