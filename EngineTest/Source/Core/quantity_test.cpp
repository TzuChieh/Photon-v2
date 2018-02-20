#include <Core/Quantity/ColorSpace.h>

#include <gtest/gtest.h>

TEST(QuantityRelatedTest, ColorSpaceConversion)
{
	ph::Vector3R color;

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

	color = ph::ColorSpace::CIE_XYZ_to_linear_sRGB({0, 0, 0});
	EXPECT_FLOAT_EQ(color.x, 0);
	EXPECT_FLOAT_EQ(color.y, 0);
	EXPECT_FLOAT_EQ(color.z, 0);

	color = ph::ColorSpace::linear_sRGB_to_CIE_XYZ({0, 0, 0});
	EXPECT_FLOAT_EQ(color.x, 0);
	EXPECT_FLOAT_EQ(color.y, 0);
	EXPECT_FLOAT_EQ(color.z, 0);
}