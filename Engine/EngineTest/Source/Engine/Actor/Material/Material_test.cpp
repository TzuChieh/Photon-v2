#include <Engine/Actor/Image/ConstantImage.h>
#include <Engine/Actor/Material/AbradedTranslucent.h>
#include <Engine/Actor/Material/BinaryMixedSurfaceMaterial.h>
#include <Engine/Actor/Material/IdealSubstance.h>
#include <Engine/Actor/Material/LayeredSurface.h>
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

TEST(MaterialTest, CookLayeredSurfaceWithConstantAndMappedLayers)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto map = TSdl<ConstantImage>::makeResource();
	map->setRaw(0.5_r);

	auto constantLayer = TSdl<SurfaceLayerInfo>::make();
	auto mappedLayer = TSdl<SurfaceLayerInfo>::make();
	mappedLayer.setRoughnessMap(map);

	auto material = TSdl<LayeredSurface>::makeResource();
	material->addLayer();
	material->setLayer(0, constantLayer);
	material->addLayer();
	material->setLayer(1, mappedLayer);

	CookedMaterial* const cookedMaterial = resources.makeMaterial(ctx.getKey(material));
	ASSERT_NE(cookedMaterial, nullptr);
	material->cook(ctx, *cookedMaterial);

	EXPECT_NE(cookedMaterial->surfaceOptics, nullptr);
}

TEST(MaterialTest, CookBinaryMixedSurfaceWithConstantFactor)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto material0 = TSdl<MatteOpaque>::makeResource();
	auto material1 = TSdl<MatteOpaque>::makeResource();
	for(const auto& material : {material0, material1})
	{
		CookedMaterial* const cookedMaterial = resources.makeMaterial(ctx.getKey(material));
		ASSERT_NE(cookedMaterial, nullptr);
		material->cook(ctx, *cookedMaterial);
	}

	auto material = TSdl<BinaryMixedSurfaceMaterial>::makeResource();
	material->setMaterials(material0, material1);
	material->setFactor(0.25_r);

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
