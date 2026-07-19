#include <Engine/Actor/Geometry/GTriangle.h>
#include <Engine/Actor/Light/AModelLight.h>
#include <Engine/Core/HitDetail.h>
#include <Engine/Core/HitProbe.h>
#include <Engine/Core/Intersection/Primitive.h>
#include <Engine/Core/Ray.h>
#include <Engine/Math/TDecomposedTransform.h>
#include <Engine/SDL/TSdl.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/CookingContext.h>
#include <Engine/World/Foundation/TransientVisualElement.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(AGeometricLightTest, ReflectedBakeHonorsGeometryNormalPolicy)
{
	// xy-plane triangle
	auto geometry = TSdl<GTriangle>::makeResource();
	geometry->setVertices({0, 0, 0}, {1, 0, 0}, {0, 1, 0});
	geometry->setNa({0, 0, 1});
	geometry->setNb({0, 0, 1});
	geometry->setNc({0, 0, 1});

	auto light = TSdl<AModelLight>::makeResource();
	light->setGeometry(geometry);

	TDecomposedTransform<real> reflectedTransform;
	reflectedTransform.scale(-1, 1, 1);
	light->setBaseTransform(reflectedTransform);

	const auto expectNormals = [&](const bool shouldFlipNg, const Vector3R& expectedNg)
	{
		light->setShouldFlipNg(shouldFlipNg);

		CookedResourceCollection resources;
		CookingContext ctx(&resources, nullptr);
		const TransientVisualElement result = light->stagelessCook(ctx);

		ASSERT_EQ(result.primitivesView.size(), 1);
		const Primitive* const primitive = result.primitivesView[0];
		ASSERT_NE(primitive, nullptr);

		const Ray ray({-0.25_r, 0.25_r, 1}, {0, 0, -1});
		HitProbe probe;
		ASSERT_TRUE(primitive->isIntersecting(ray, probe));

		HitDetail detail;
		probe.calcHitDetail(ray, &detail);
		EXPECT_EQ(detail.getGeometryNormal(), expectedNg);
		EXPECT_EQ(detail.getShadingNormal(), Vector3R(0, 0, 1));
	};

	expectNormals(false, Vector3R(0, 0, 1));
	expectNormals(true, Vector3R(0, 0, -1));
}
