#include <Engine/Actor/AModel.h>
#include <Engine/Actor/ATransformedInstance.h>
#include <Engine/Actor/Basic/TransformInfo.h>
#include <Engine/Actor/Geometry/GSphere.h>
#include <Engine/Actor/Material/MatteOpaque.h>
#include <Engine/Core/Ray.h>
#include <Engine/SDL/TSdl.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/CookingContext.h>
#include <Engine/World/Foundation/TransientResourceCache.h>
#include <Engine/World/Foundation/TransientVisualElement.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(ATransformedInstanceTest, CooksTranslatedInstance)
{
	auto geometry = TSdl<GSphere>::makeResource();
	auto material = TSdl<MatteOpaque>::makeResource();
	auto source = TSdl<AModel>::makeResource();
	source->setGeometry(geometry);
	source->setMaterial(material);
	source->setIsInstantiableHint(true);

	auto transform = TSdl<TransformInfo>::make();
	transform.setPos(2, 0, 0);
	auto instance = TSdl<ATransformedInstance>::makeResource();
	instance->setSource(source);
	instance->setTransforms({transform});

	CookedResourceCollection resources;
	TransientResourceCache cache;
	CookingContext ctx(&resources, &cache);
	geometry->cook(ctx, *resources.makeGeometry(ctx.getKey(geometry)));
	material->cook(ctx, *resources.makeMaterial(ctx.getKey(material)));
	cache.makeVisualElement(source->getId(), source->stagelessCook(ctx));

	const TransientVisualElement result = instance->stagelessCook(ctx);

	ASSERT_EQ(result.intersectables.size(), 1);
	const Ray instanceRay({2, 0, 3}, {0, 0, -1});
	EXPECT_TRUE(result.intersectables.front()->isOccluding(instanceRay));
}
