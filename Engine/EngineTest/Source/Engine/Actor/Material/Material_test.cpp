#include <Engine/Actor/Material/MatteOpaque.h>
#include <Engine/SDL/TSdl.h>
#include <Engine/World/Foundation/CookedMaterial.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/CookingContext.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(MaterialTest, GetCookedDoesNotCreateMaterial)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto material = TSdl<MatteOpaque>::makeResource();
	EXPECT_EQ(ctx.getCooked(material), nullptr);
}

TEST(MaterialTest, CookFillsProvidedStorage)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto material = TSdl<MatteOpaque>::makeResource();
	CookedMaterial* const cookedMaterial = resources.makeMaterial(ctx.getKey(material));
	ASSERT_NE(cookedMaterial, nullptr);
	material->cook(ctx, *cookedMaterial);

	EXPECT_EQ(ctx.getCooked(material), cookedMaterial);
	EXPECT_NE(cookedMaterial->surfaceOptics, nullptr);
}
