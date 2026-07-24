#include <Engine/Actor/Image/ColorRemapImage.h>
#include <Engine/Actor/Image/ConstantImage.h>
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

TEST(ColorRemapImageTest, RemapsColors)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);
	const SampleLocation sampleLocation(Vector2R(0), EColorUsage::Raw);

	const auto makeLinearSrgbColor =
		[](const Vector3R& color)
		{
			auto image = std::make_shared<ConstantImage>();
			image->setColor(color, EColorSpace::Linear_sRGB);
			return image;
		};

	// Identity LUT preserves the input spectrum
	{
		auto input = std::make_shared<ConstantImage>();
		input->setRaw(0.5_r);

		auto image = TSdl<ColorRemapImage>::make();
		image.setInput(input);
		image.setRgbValues({{0, 0, 0}, {1, 1, 1}});

		const auto texture = image.genColorTexture(ctx);
		Spectrum sampled;
		texture->sample(sampleLocation, &sampled);
		EXPECT_EQ(sampled, Spectrum(0.5_r));
	}

	// Upper extrapolation maps 2 to 3, then factor 0.5 blends to 2.5
	{
		auto image = TSdl<ColorRemapImage>::make();
		image.setInput(makeLinearSrgbColor({2.0_r, 2.0_r, 2.0_r}));
		image.setRgbValues({{0, 0, 0}, {1, 1, 1}});
		image.setUpperExtrapolationSlope({2, 2, 2});
		image.setFactor(0.5_r);

		const auto texture = image.genColorTexture(ctx);
		Spectrum sampledSpectrum;
		texture->sample(sampleLocation, &sampledSpectrum);
		const auto sampled = sampledSpectrum.toLinearSRGB(EColorUsage::Raw);
		EXPECT_NEAR(sampled[0], 2.5_r, 1e-5_r);
		EXPECT_NEAR(sampled[1], 2.5_r, 1e-5_r);
		EXPECT_NEAR(sampled[2], 2.5_r, 1e-5_r);
	}

	// Lower extrapolation maps -1 to -2, then linked factor 0.5 blends to -1.5
	{
		auto factorMap = std::make_shared<ConstantImage>();
		factorMap->setRaw(0.5_r);

		auto image = TSdl<ColorRemapImage>::make();
		image.setInput(makeLinearSrgbColor({-1.0_r, -1.0_r, -1.0_r}));
		image.setRgbValues({{0, 0, 0}, {1, 1, 1}});
		image.setLowerExtrapolationSlope({2, 2, 2});
		image.setFactor(0.0_r);
		image.setFactorMap(factorMap);

		const auto texture = image.genNumericTexture(ctx);
		Image::NumericType sampled;
		texture->sample(sampleLocation, &sampled);
		EXPECT_NEAR(sampled[0], -1.5_r, 1e-5_r);
		EXPECT_NEAR(sampled[1], -1.5_r, 1e-5_r);
		EXPECT_NEAR(sampled[2], -1.5_r, 1e-5_r);
	}
}
