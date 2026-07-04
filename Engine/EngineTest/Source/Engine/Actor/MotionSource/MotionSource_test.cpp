#include <Engine/Actor/MotionSource/ConstantVelocityMotion.h>
#include <Engine/Core/Quantity/TimeStep.h>
#include <Engine/World/Foundation/CookedMotion.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/CookingConfig.h>
#include <Engine/World/Foundation/CookingContext.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(MotionSourceTest, GetCookedDoesNotCreateMotion)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	ConstantVelocityMotion motion;
	EXPECT_EQ(ctx.getCooked(motion), nullptr);
}

TEST(MotionSourceTest, CookFillsProvidedStorage)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	ConstantVelocityMotion motion;
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

	ConstantVelocityMotion motion;
	CookedMotion* const defaultCooked = resources.makeMotion(ctx.getKey(motion));
	ASSERT_NE(defaultCooked, nullptr);
	motion.cook(ctx, *defaultCooked);

	CookingConfig shiftedConfig;
	shiftedConfig.timeStep = TimeStep(1, 1.0_r, 1.0_r);
	ctx.setConfig(shiftedConfig);

	CookedMotion* const shiftedCooked = resources.makeMotion(ctx.getKey(motion));
	ASSERT_NE(shiftedCooked, nullptr);
	motion.cook(ctx, *shiftedCooked);

	EXPECT_EQ(ctx.getCooked(motion), shiftedCooked);
	EXPECT_NE(shiftedCooked, defaultCooked);

	ctx.setConfig(CookingConfig());
	EXPECT_EQ(ctx.getCooked(motion), defaultCooked);
}
