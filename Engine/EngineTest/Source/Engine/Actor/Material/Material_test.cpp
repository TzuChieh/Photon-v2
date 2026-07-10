#include <Engine/Actor/Image/ConstantImage.h>
#include <Engine/Actor/Material/AbradedTranslucent.h>
#include <Engine/Actor/Material/IdealSubstance.h>
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

TEST(MaterialTest, CookMappedAbradedTranslucent)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto map = TSdl<ConstantImage>::makeResource();
	map->setRaw(0.5_r);

	auto material = TSdl<AbradedTranslucent>::makeResource();
	material->setRoughnessMap(map);
	material->setRoughnessVMap(map);

	CookedMaterial* const cookedMaterial = resources.makeMaterial(ctx.getKey(material));
	ASSERT_NE(cookedMaterial, nullptr);
	material->cook(ctx, *cookedMaterial);

	EXPECT_NE(cookedMaterial->surfaceOptics, nullptr);
}

TEST(MaterialTest, CookMappedMetallicIdealSubstance)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto map = TSdl<ConstantImage>::makeResource();
	map->setRaw(0.5_r);

	auto material = TSdl<IdealSubstance>::makeResource();
	material->setSubstance(EIdealSubstance::MetallicReflector);
	material->setF0Map(map);
	material->setReflectionScaleMap(map);

	CookedMaterial* const cookedMaterial = resources.makeMaterial(ctx.getKey(material));
	ASSERT_NE(cookedMaterial, nullptr);
	material->cook(ctx, *cookedMaterial);

	EXPECT_NE(cookedMaterial->surfaceOptics, nullptr);
}

TEST(MaterialTest, CookMappedDielectricIdealSubstance)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto map = TSdl<ConstantImage>::makeResource();
	map->setRaw(0.5_r);

	auto material = TSdl<IdealSubstance>::makeResource();
	material->setSubstance(EIdealSubstance::Dielectric);
	material->setReflectionScaleMap(map);
	material->setTransmissionScaleMap(map);

	CookedMaterial* const cookedMaterial = resources.makeMaterial(ctx.getKey(material));
	ASSERT_NE(cookedMaterial, nullptr);
	material->cook(ctx, *cookedMaterial);

	EXPECT_NE(cookedMaterial->surfaceOptics, nullptr);
}
