#include <Engine/Core/Texture/Pixel/pixel_texture_basics.h>

#include <gtest/gtest.h>

#include <stdexcept>

using namespace ph;
using namespace ph::math;
using namespace ph::pixel_texture;

TEST(PixelTextureBasicsTest, WrapModesMapUvToUnitRange)
{
	// Repeat mode should keep only the fractional part and wrap negatives back into [0, 1].
	EXPECT_DOUBLE_EQ(0.25, uv_to_st_scalar(0.25, EWrapMode::Repeat));
	EXPECT_DOUBLE_EQ(0.25, uv_to_st_scalar(1.25, EWrapMode::Repeat));
	EXPECT_DOUBLE_EQ(0.75, uv_to_st_scalar(-0.25, EWrapMode::Repeat));

	// Clamp-to-edge should pin anything outside the valid range onto the nearest edge.
	EXPECT_DOUBLE_EQ(0.00, uv_to_st_scalar(-0.25, EWrapMode::ClampToEdge));
	EXPECT_DOUBLE_EQ(0.25, uv_to_st_scalar(0.25, EWrapMode::ClampToEdge));
	EXPECT_DOUBLE_EQ(1.00, uv_to_st_scalar(1.25, EWrapMode::ClampToEdge));

	// Flipped clamp first mirrors the axis, then clamps to the nearest edge.
	EXPECT_DOUBLE_EQ(1.00, uv_to_st_scalar(0.00, EWrapMode::FlippedClampToEdge));
	EXPECT_DOUBLE_EQ(0.75, uv_to_st_scalar(0.25, EWrapMode::FlippedClampToEdge));
	EXPECT_DOUBLE_EQ(0.00, uv_to_st_scalar(1.25, EWrapMode::FlippedClampToEdge));

	// The 2D overload should simply apply the selected wrap rule per axis.
	const Vector2D st = uv_to_st(Vector2D(-0.25, 1.25), EWrapMode::Repeat, EWrapMode::ClampToEdge);
	EXPECT_DOUBLE_EQ(0.75, st.x());
	EXPECT_DOUBLE_EQ(1.00, st.y());
}

TEST(PixelTextureBasicsTest, PixelLayoutsReportComponentCount)
{
	// Single-channel layouts should report exactly one stored element.
	EXPECT_EQ(1, num_pixel_elements(EPixelLayout::R));
	EXPECT_EQ(1, num_pixel_elements(EPixelLayout::Monochromatic));
	EXPECT_EQ(1, num_pixel_elements(EPixelLayout::A));

	// Packed color layouts should match the number of explicitly stored channels.
	EXPECT_EQ(2, num_pixel_elements(EPixelLayout::RG));
	EXPECT_EQ(3, num_pixel_elements(EPixelLayout::RGB));
	EXPECT_EQ(3, num_pixel_elements(EPixelLayout::BGR));
	EXPECT_EQ(4, num_pixel_elements(EPixelLayout::RGBA));
	EXPECT_EQ(4, num_pixel_elements(EPixelLayout::ABGR));
}

TEST(PixelTextureBasicsTest, AlphaChannelIndexMatchesLayoutOrdering)
{
	// Layouts with alpha should report where that alpha value is stored.
	EXPECT_EQ(0, alpha_channel_index(EPixelLayout::A));
	EXPECT_EQ(0, alpha_channel_index(EPixelLayout::ARGB));
	EXPECT_EQ(0, alpha_channel_index(EPixelLayout::ABGR));
	EXPECT_EQ(3, alpha_channel_index(EPixelLayout::RGBA));
	EXPECT_EQ(3, alpha_channel_index(EPixelLayout::BGRA));

	// Layouts without alpha should reject the query instead of inventing an index.
	EXPECT_THROW(alpha_channel_index(EPixelLayout::RGB), std::invalid_argument);
	EXPECT_THROW(alpha_channel_index(EPixelLayout::Monochromatic), std::invalid_argument);
}
