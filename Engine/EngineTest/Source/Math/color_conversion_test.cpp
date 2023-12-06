#include <Math/Color/color_spaces.h>
#include <Math/Color/spectral_samples.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(ColorConversionTest, SrgbCieXyzInterConversion)
{
	constexpr real ACCEPTABLE_ERROR = 0.0003_r;

	TTristimulusValues<real> color;

	color = sRGB_nonlinear_to_linear<real>({0, 0, 0});
	EXPECT_NEAR(color[0], 0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[1], 0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[2], 0, ACCEPTABLE_ERROR);

	color = sRGB_linear_to_nonlinear<real>({0, 0, 0});
	EXPECT_NEAR(color[0], 0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[1], 0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[2], 0, ACCEPTABLE_ERROR);

	color = sRGB_nonlinear_to_linear<real>({1, 1, 1});
	EXPECT_NEAR(color[0], 1, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[1], 1, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[2], 1, ACCEPTABLE_ERROR);

	color = sRGB_linear_to_nonlinear<real>({1, 1, 1});
	EXPECT_NEAR(color[0], 1, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[1], 1, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[2], 1, ACCEPTABLE_ERROR);

	using LinearSRGBDef = TColorSpaceDefinition<EColorSpace::Linear_sRGB, real>;

	color = LinearSRGBDef::fromCIEXYZ({0, 0, 0});
	EXPECT_NEAR(color[0], 0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[1], 0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[2], 0, ACCEPTABLE_ERROR);

	color = LinearSRGBDef::toCIEXYZ({0, 0, 0});
	EXPECT_NEAR(color[0], 0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[1], 0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[2], 0, ACCEPTABLE_ERROR);

	const TTristimulusValues<real> normalizedD65_XYZ{0.95047_r, 1.0_r, 1.08883_r};

	color = LinearSRGBDef::fromCIEXYZ(normalizedD65_XYZ);
	EXPECT_NEAR(color[0], 1, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[1], 1, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[2], 1, ACCEPTABLE_ERROR);

	color = LinearSRGBDef::toCIEXYZ({1, 1, 1});
	EXPECT_NEAR(color[0], normalizedD65_XYZ[0], ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[1], normalizedD65_XYZ[1], ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[2], normalizedD65_XYZ[2], ACCEPTABLE_ERROR);
}

TEST(ColorConversionTest, SpectrumToCieXyzConversion)
{
	constexpr real ACCEPTABLE_ERROR = 0.0003_r;

	TTristimulusValues<real> color;

	color = spectral_samples_to_CIE_XYZ<real, DefaultSpectralSampleProps, EReferenceWhite::D65>(
		resample_illuminant_D65<real>(), EColorUsage::EMR);
	EXPECT_NEAR(color[0], 0.95047_r, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[1], 1.00000_r, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[2], 1.08883_r, ACCEPTABLE_ERROR);

	color = spectral_samples_to_CIE_XYZ<real>(constant_spectral_samples<real>(0), EColorUsage::EMR);
	EXPECT_NEAR(color[0], 0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[1], 0, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[2], 0, ACCEPTABLE_ERROR);

	color = spectral_samples_to_CIE_XYZ<real, DefaultSpectralSampleProps, EReferenceWhite::E>(
		resample_illuminant_E<real>(), EColorUsage::EMR);
	EXPECT_NEAR(color[0], 1.0_r, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[1], 1.0_r, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[2], 1.0_r, ACCEPTABLE_ERROR);

	color = spectral_samples_to_CIE_XYZ<real>(constant_spectral_samples<real>(1), EColorUsage::ECF);
	EXPECT_NEAR(color[0], 1.0_r, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[1], 1.0_r, ACCEPTABLE_ERROR);
	EXPECT_NEAR(color[2], 1.0_r, ACCEPTABLE_ERROR);
}
