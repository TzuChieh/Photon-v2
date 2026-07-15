#include <Engine/Actor/Image/ConstantImage.h>
#include <Engine/Actor/Image/LuminanceImage.h>
#include <Engine/Core/Texture/SampleLocation.h>
#include <Engine/Math/Color/color_enums.h>
#include <Engine/Math/Color/color_spaces.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/CookingContext.h>

#include <gtest/gtest.h>

#include <memory>

using namespace ph;
using namespace ph::math;

TEST(LuminanceImageTest, HonorsInputColorSpace)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto input = std::make_shared<ConstantImage>();
	input->setColor(Vector3D(0.2, 0.4, 0.1), EColorSpace::CIE_XYZ);

	LuminanceImage image;
	image.setInput(input);
	const auto texture = image.genRealTexture(ctx);

	real sampled;
	texture->sample(SampleLocation(Vector2R(0), EColorUsage::Raw), &sampled);
	// CIE XYZ defines relative luminance as its Y component
	EXPECT_NEAR(sampled, 0.4_r, 1e-6_r);
}

TEST(LuminanceImageTest, HonorsSampleColorUsage)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto input = std::make_shared<ConstantImage>();
	input->setRaw(2.0);

	LuminanceImage image;
	image.setInput(input);
	const auto texture = image.genRealTexture(ctx);

	real sampled;
	texture->sample(SampleLocation(Vector2R(0), EColorUsage::ECF), &sampled);
	// ECF clamps values to [0, 1]
	EXPECT_EQ(sampled, 1.0_r);
}

