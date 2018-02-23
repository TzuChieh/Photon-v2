#include <Core/Quantity/ColorSpace.h>
#include <Core/Quantity/SpectralStrength/TSampledSpectralStrength.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

TEST(ColorConversionTest, SrgbCieXyzInterConversion)
{
	using namespace ph;

	Vector3R color;

	color = ph::ColorSpace::sRGB_to_linear_sRGB({0, 0, 0});
	EXPECT_FLOAT_EQ(color.x, 0);
	EXPECT_FLOAT_EQ(color.y, 0);
	EXPECT_FLOAT_EQ(color.z, 0);

	color = ph::ColorSpace::linear_sRGB_to_sRGB({0, 0, 0});
	EXPECT_FLOAT_EQ(color.x, 0);
	EXPECT_FLOAT_EQ(color.y, 0);
	EXPECT_FLOAT_EQ(color.z, 0);

	color = ph::ColorSpace::sRGB_to_linear_sRGB({1, 1, 1});
	EXPECT_FLOAT_EQ(color.x, 1);
	EXPECT_FLOAT_EQ(color.y, 1);
	EXPECT_FLOAT_EQ(color.z, 1);

	color = ph::ColorSpace::linear_sRGB_to_sRGB({1, 1, 1});
	EXPECT_FLOAT_EQ(color.x, 1);
	EXPECT_FLOAT_EQ(color.y, 1);
	EXPECT_FLOAT_EQ(color.z, 1);

	color = ph::ColorSpace::CIE_XYZ_D65_to_linear_sRGB({0, 0, 0});
	EXPECT_FLOAT_EQ(color.x, 0);
	EXPECT_FLOAT_EQ(color.y, 0);
	EXPECT_FLOAT_EQ(color.z, 0);

	color = ph::ColorSpace::linear_sRGB_to_CIE_XYZ_D65({0, 0, 0});
	EXPECT_FLOAT_EQ(color.x, 0);
	EXPECT_FLOAT_EQ(color.y, 0);
	EXPECT_FLOAT_EQ(color.z, 0);

	ph::Vector3R normalizedD65_XYZ(0.95047_r, 1.0_r, 1.08883_r);

	color = ph::ColorSpace::CIE_XYZ_D65_to_linear_sRGB(normalizedD65_XYZ);
	EXPECT_NEAR(color.x, 1, 0.001_r);
	EXPECT_NEAR(color.y, 1, 0.001_r);
	EXPECT_NEAR(color.z, 1, 0.001_r);

	color = ph::ColorSpace::linear_sRGB_to_CIE_XYZ_D65({1, 1, 1});
	EXPECT_NEAR(color.x, normalizedD65_XYZ.x, 0.001_r);
	EXPECT_NEAR(color.y, normalizedD65_XYZ.y, 0.001_r);
	EXPECT_NEAR(color.z, normalizedD65_XYZ.z, 0.001_r);
}

TEST(ColorConversionTest, SpectrumToCieXyzConversion)
{
	using namespace ph;

	Vector3R color;

	color = ColorSpace::SPD_to_CIE_XYZ_D65(ColorSpace::get_D65_SPD());
	EXPECT_NEAR(color.x, 0.95047_r, 0.001_r);
	EXPECT_NEAR(color.y, 1.00000_r, 0.001_r);
	EXPECT_NEAR(color.z, 1.08883_r, 0.001_r);

	color = ColorSpace::SPD_to_CIE_XYZ_D65(SampledSpectralStrength(0));
	EXPECT_FLOAT_EQ(color.x, 0);
	EXPECT_FLOAT_EQ(color.y, 0);
	EXPECT_FLOAT_EQ(color.z, 0);
}