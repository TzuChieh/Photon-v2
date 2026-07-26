#include "engine_test_util.h"

#include <Engine/Actor/Image/AttributeImage.h>
#include <Engine/Core/ECoordSys.h>
#include <Engine/Core/HitDetail.h>
#include <Engine/Core/Intersection/GeometryInfo.h>
#include <Engine/Core/Intersection/PrimitiveMetadata.h>
#include <Engine/Core/SurfaceHit.h>
#include <Engine/Core/Texture/SampleLocation.h>
#include <Engine/Math/TVector3.h>
#include <Engine/Math/TVector4.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/CookingContext.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(AttributeImageTest, ProducesUvwFromGeometryBound)
{
	GeometryInfo geometryInfo;
	geometryInfo.localAABB = AABB3D(
		Vector3R(-2.0_r, 0.0_r, 2.0_r),
		Vector3R( 2.0_r, 4.0_r, 6.0_r));
	PrimitiveMetadata metadata;
	metadata.setGeometryInfo(&geometryInfo);

	HitDetail detail;
	detail.hitInfo(ECoordSys::Local).setAttributes(
		Vector3R(-1.0_r, 2.0_r, 5.0_r), Vector3R(0, 1, 0));
	const SurfaceHit X(
		Ray(),
		HitProbe(),
		detail,
		&metadata,
		ESurfaceHitReason::Unknown,
		false);

	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);
	AttributeImage image;
	image.setKind(EAttributeKind::UvwFromGeometryBound);
	const auto texture = image.genVector4RTexture(ctx);

	Vector4R sampled;
	texture->sample(SampleLocation(&X), &sampled);

	PH_EXPECT_REAL_EQ(0.25_r, sampled.x());
	PH_EXPECT_REAL_EQ(0.50_r, sampled.y());
	PH_EXPECT_REAL_EQ(0.75_r, sampled.z());
	PH_EXPECT_REAL_EQ(0.00_r, sampled.w());
}
