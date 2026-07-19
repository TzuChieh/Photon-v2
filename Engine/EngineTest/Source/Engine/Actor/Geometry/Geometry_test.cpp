#include <Engine/Actor/Geometry/GCuboid.h>
#include <Engine/World/Foundation/CookedGeometry.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/GeometryCookingConfig.h>
#include <Engine/World/Foundation/CookingContext.h>
#include <Engine/SDL/TSdl.h>

#include <Common/exceptions.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(GeometryTest, GetCookedDoesNotCreateGeometry)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto cuboid = TSdl<GCuboid>::makeResource();

	EXPECT_EQ(ctx.getCooked(cuboid), nullptr);
}

TEST(GeometryTest, CookFillsProvidedStorage)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto cuboid = TSdl<GCuboid>::makeResource();
	cuboid->setSize(1);

	CookedGeometry* const cookedGeometry = resources.makeGeometry(ctx.getKey(cuboid));
	ASSERT_NE(cookedGeometry, nullptr);
	cuboid->cook(ctx, *cookedGeometry);

	EXPECT_EQ(ctx.getCooked(cuboid), cookedGeometry);
	EXPECT_FALSE(cookedGeometry->primitives.empty());
}

TEST(GeometryTest, CookKeepsDistinctConfigVariants)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto cuboid = TSdl<GCuboid>::makeResource();
	cuboid->setSize(1);

	// Cook with default config
	CookedGeometry* const defaultCooked = resources.makeGeometry(ctx.getKey(cuboid));
	ASSERT_NE(defaultCooked, nullptr);
	cuboid->cook(ctx, *defaultCooked);

	GeometryCookingConfig triangulatedConfig;
	triangulatedConfig.forceTriangulated = 1;
	ctx.setGeometryConfig(triangulatedConfig);

	// Cook with triangulated config
	CookedGeometry* const triangulatedCooked = resources.makeGeometry(ctx.getKey(cuboid));
	ASSERT_NE(triangulatedCooked, nullptr);
	cuboid->cook(ctx, *triangulatedCooked);

	EXPECT_EQ(ctx.getCooked(cuboid), triangulatedCooked);
	EXPECT_NE(triangulatedCooked, defaultCooked);

	ctx.setGeometryConfig(GeometryCookingConfig());
	EXPECT_EQ(ctx.getCooked(cuboid), defaultCooked);
}

TEST(GeometryTest, MakeGeometryRejectsDuplicateKey)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto cuboid = TSdl<GCuboid>::makeResource();
	const auto key = ctx.getKey(cuboid);

	// First time OK
	ASSERT_NE(resources.makeGeometry(key), nullptr);

	// Second time throw
	EXPECT_THROW(resources.makeGeometry(key), IllegalOperationException);
}
