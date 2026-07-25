#include <Engine/Actor/Image/ConstantImage.h>
#include <Engine/Core/Texture/SampleLocation.h>
#include <Engine/Math/Color/color_spaces.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/CookingContext.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(ConstantImageTest, StoresRawArray)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	ConstantImage image;
	image.setRaw(Vector3R(0.2_r, 0.4_r, 0.6_r));
	const auto texture = image.genColorTexture(ctx);

	Spectrum sampled;
	texture->sample(SampleLocation(Vector3R(0)), &sampled);
	EXPECT_EQ(sampled[0], 0.2_r);
	EXPECT_EQ(sampled[1], 0.4_r);
	EXPECT_EQ(sampled[2], 0.6_r);
	for(int componentIdx = 3; componentIdx < Spectrum::NUM_VALUES; ++componentIdx)
	{
		EXPECT_EQ(sampled[componentIdx], 0.0_r);
	}
}

TEST(ConstantImageTest, BroadcastsRawScalar)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	ConstantImage image;
	image.setRaw(0.25_r);
	const auto texture = image.genColorTexture(ctx);

	Spectrum sampled;
	texture->sample(SampleLocation(Vector3R(0)), &sampled);
	for(const ColorValue value : sampled.getColorValues())
	{
		EXPECT_EQ(value, 0.25_r);
	}
}
