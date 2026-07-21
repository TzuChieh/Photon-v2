#include <Engine/Actor/Image/ConstantImage.h>
#include <Engine/Actor/Image/HsvImage.h>
#include <Engine/Core/Texture/SampleLocation.h>
#include <Engine/Math/Color/color_enums.h>
#include <Engine/Math/Color/Spectrum.h>
#include <Engine/Math/TVector2.h>
#include <Engine/Math/TVector3.h>
#include <Engine/SDL/TSdl.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/CookingContext.h>

#include <gtest/gtest.h>

#include <memory>

using namespace ph;
using namespace ph::math;

namespace
{

Spectrum sample_spectrum(HsvImage& image, const CookingContext& ctx)
{
	const auto texture = image.genColorTexture(ctx);
	Spectrum sampled;
	texture->sample(SampleLocation(Vector2R(0), EColorUsage::Raw), &sampled);
	return sampled;
}

std::shared_ptr<ConstantImage> make_linear_srgb_color(const Vector3R& color)
{
	auto image = std::make_shared<ConstantImage>();
	image->setColor(color, EColorSpace::Linear_sRGB);
	return image;
}

}// end anonymous namespace

TEST(HsvImageTest, AppliesHueOffset)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);
	auto image = TSdl<HsvImage>::make();
	image.setInput(make_linear_srgb_color(Vector3R(1.0_r, 0.0_r, 0.0_r)));
	image.setHue(1.0_r / 3.0_r);

	const auto sampled = sample_spectrum(image, ctx).toLinearSRGB(EColorUsage::Raw);
	EXPECT_NEAR(sampled[0], 0.0_r, 1e-5_r);
	EXPECT_NEAR(sampled[1], 1.0_r, 1e-5_r);
	EXPECT_NEAR(sampled[2], 0.0_r, 1e-5_r);
}

TEST(HsvImageTest, ZeroAmountPreservesInputSpectrum)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);
	auto image = TSdl<HsvImage>::make();
	image.setInput(make_linear_srgb_color(Vector3R(0.8_r, 0.3_r, 0.1_r)));
	image.setHue(2.0_r / 3.0_r);
	image.setAmount(0.0_r);

	Spectrum expected;
	expected.setLinearSRGB({0.8_r, 0.3_r, 0.1_r}, EColorUsage::Raw);

	const auto sampled = sample_spectrum(image, ctx);
	EXPECT_EQ(sampled.getColorValues(), expected.getColorValues());
}
