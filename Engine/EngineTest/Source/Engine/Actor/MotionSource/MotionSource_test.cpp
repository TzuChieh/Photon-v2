#include <Engine/Actor/MotionSource/ConstantVelocityMotion.h>
#include <Engine/Core/Quantity/TimeStep.h>
#include <Engine/SDL/TSdl.h>
#include <Engine/World/Foundation/CookedMotion.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/CommonCookingConfig.h>
#include <Engine/World/Foundation/CookingContext.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(MotionSourceTest, GetCookedDoesNotCreateMotion)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto motion = TSdl<ConstantVelocityMotion>::make();
	EXPECT_EQ(ctx.getCooked(motion), nullptr);
}

TEST(MotionSourceTest, CookFillsProvidedStorage)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto motion = TSdl<ConstantVelocityMotion>::make();
	CookedMotion* const cookedMotion = resources.makeMotion(ctx.getKey(motion));
	ASSERT_NE(cookedMotion, nullptr);
	motion.cook(ctx, *cookedMotion);

	EXPECT_EQ(ctx.getCooked(motion), cookedMotion);
	EXPECT_NE(cookedMotion->localToWorld, nullptr);
	EXPECT_NE(cookedMotion->worldToLocal, nullptr);
}

TEST(MotionSourceTest, CookKeepsDistinctConfigVariants)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto motion = TSdl<ConstantVelocityMotion>::make();
	CookedMotion* const defaultCooked = resources.makeMotion(ctx.getKey(motion));
	ASSERT_NE(defaultCooked, nullptr);
	motion.cook(ctx, *defaultCooked);

	CommonCookingConfig shiftedConfig;
	shiftedConfig.timeStep = TimeStep(1, 1.0_r, 1.0_r);
	ctx.setCommonConfig(shiftedConfig);

	CookedMotion* const shiftedCooked = resources.makeMotion(ctx.getKey(motion));
	ASSERT_NE(shiftedCooked, nullptr);
	motion.cook(ctx, *shiftedCooked);

	EXPECT_EQ(ctx.getCooked(motion), shiftedCooked);
	EXPECT_NE(shiftedCooked, defaultCooked);

	ctx.setCommonConfig(CommonCookingConfig());
	EXPECT_EQ(ctx.getCooked(motion), defaultCooked);
}
