#include <Engine/Actor/SDLExtension/sdl_spectrum_io.h>
#include <Engine/Math/Color/color_spaces.h>
#include <Engine/Math/Color/spectral_samples.h>
#include <Engine/SDL/sdl_exceptions.h>

#include <gtest/gtest.h>

#include <array>
#include <string>

using namespace ph;
using namespace ph::math;

TEST(SdlSpectrumIoTest, UntaggedColorUsesLinearSrgb)
{
	const TristimulusValues input = {0.2_r, 0.4_r, 0.6_r};
	const Spectrum loaded = sdl::load_spectrum("0.2 0.4 0.6", "", EColorUsage::ECF);
	const Spectrum expected = Spectrum().setLinearSRGB(input, EColorUsage::ECF);

	EXPECT_EQ(loaded.getColorValues(), expected.getColorValues());
}

TEST(SdlSpectrumIoTest, LoadsRawTriple)
{
	const TristimulusValues input = {0.2_r, 0.4_r, 0.6_r};

	if constexpr(is_tristimulus(Spectrum::getColorSpace()))
	{
		const Spectrum loaded = sdl::load_spectrum(
			"0.2 0.4 0.6", EColorSpace::ACEScg, EColorUsage::Raw);
		EXPECT_EQ(loaded[0], input[0]);
		EXPECT_EQ(loaded[1], input[1]);
		EXPECT_EQ(loaded[2], input[2]);
	}
	else
	{
		const Spectrum untagged = sdl::load_spectrum(
			"0.2 0.4 0.6", EColorSpace::Unspecified, EColorUsage::Raw);
		const Spectrum expectedUntagged = Spectrum().setLinearSRGB(input, EColorUsage::Raw);
		EXPECT_EQ(untagged.getColorValues(), expectedUntagged.getColorValues());

		const Spectrum tagged = sdl::load_spectrum(
			"0.2 0.4 0.6", EColorSpace::CIE_XYZ, EColorUsage::Raw);
		const Spectrum expectedTagged = Spectrum().setTransformed<EColorSpace::CIE_XYZ>(
			input, EColorUsage::Raw);
		EXPECT_EQ(tagged.getColorValues(), expectedTagged.getColorValues());
	}
}

TEST(SdlSpectrumIoTest, BroadcastsRawScalar)
{
	const Spectrum loaded = sdl::load_spectrum("0.25", "", EColorUsage::Raw);

	for(const ColorValue value : loaded.getColorValues())
	{
		EXPECT_EQ(value, 0.25_r);
	}
}

TEST(SdlSpectrumIoTest, LoadsWavelengthValuePairs)
{
	// Wavelengths come first, followed by their corresponding values.
	const Spectrum loaded = sdl::load_spectrum(
		"400 700 0.25 0.75", EColorSpace::Spectral, EColorUsage::EMR);

	const std::array<ColorValue, 2> wavelengths = {400.0_r, 700.0_r};
	const std::array<ColorValue, 2> values = {0.25_r, 0.75_r};
	const SampledSpectrum sampled(math::resample_spectral_samples<ColorValue>(wavelengths, values));
	const Spectrum expected = Spectrum().setSpectral(sampled.getColorValues(), EColorUsage::EMR);
	
	EXPECT_EQ(loaded.getColorValues(), expected.getColorValues());
}

TEST(SdlSpectrumIoTest, RejectsMismatchedTag)
{
	EXPECT_THROW(
		sdl::load_spectrum("0.2 0.4 0.6", EColorSpace::Spectral, EColorUsage::Raw),
		SdlLoadError);
	EXPECT_THROW(
		sdl::load_spectrum("400 700 0.25 0.75", EColorSpace::Linear_sRGB, EColorUsage::Raw),
		SdlLoadError);
}

TEST(SdlSpectrumIoTest, RoundTripsWorkingColorSpaceValues)
{
	Spectrum spectrum;
	for(int componentIdx = 0; componentIdx < Spectrum::NUM_VALUES; ++componentIdx)
	{
		spectrum[componentIdx] = 0.1_r + 0.01_r * componentIdx;
	}

	std::string expectedTag;
	if constexpr(Spectrum::getColorSpace() == EColorSpace::Linear_sRGB)
	{
		expectedTag = "LSRGB";
	}
	else if constexpr(Spectrum::getColorSpace() == EColorSpace::ACEScg)
	{
		expectedTag = "ACEScg";
	}
	else
	{
		expectedTag = "SPD";
	}

	// Save then load under each usage should load back same value

	constexpr std::array usages = {
		EColorUsage::Raw, EColorUsage::EMR, EColorUsage::ECF};
	for(const EColorUsage usage : usages)
	{
		std::string savedValue;
		std::string savedTag;
		sdl::save_spectrum(spectrum, usage, savedValue, savedTag);

		ASSERT_EQ(savedValue.front(), '"');
		ASSERT_EQ(savedValue.back(), '"');
		EXPECT_EQ(savedTag, expectedTag);

		const auto savedNumbers = savedValue.substr(1, savedValue.size() - 2);
		const Spectrum loaded = sdl::load_spectrum(savedNumbers, savedTag, usage);
		EXPECT_EQ(loaded.getColorValues(), spectrum.getColorValues());
	}
}
