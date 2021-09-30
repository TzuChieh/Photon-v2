#include <Core/Texture/TNearestPixelTex2D.h>
#include <Core/Texture/TBilinearPixelTex2D.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(PixelBasedTextureTest, NearestFilteredTexture)
{
	using Frame = TFrame<int, 1>;

	auto uv = [](const real u, const real v)
	{
		return SampleLocation(Vector3R(u, v, 0), math::EColorUsage::RAW);
	};

	// trial 1

	Frame frame1(2, 2);
	frame1.setPixel(0, 0, Frame::Pixel(1));
	frame1.setPixel(0, 1, Frame::Pixel(2));
	frame1.setPixel(1, 0, Frame::Pixel(3));
	frame1.setPixel(1, 1, Frame::Pixel(4));

	TNearestPixelTex2D<int, 1> texture1(frame1);
	texture1.setWrapMode(ETexWrapMode::REPEAT);

	TTexPixel<int, 1> pixel;

	texture1.sample(uv(0.0_r, 0.0_r), &pixel);
	EXPECT_EQ(pixel[0], 1);

	texture1.sample(uv(0.4_r, 0.4_r), &pixel);
	EXPECT_EQ(pixel[0], 1);

	texture1.sample(uv(0.6_r, 0.4_r), &pixel);
	EXPECT_EQ(pixel[0], 3);

	texture1.sample(uv(0.2_r, 0.8_r), &pixel);
	EXPECT_EQ(pixel[0], 2);

	texture1.sample(uv(0.9_r, 0.9_r), &pixel);
	EXPECT_EQ(pixel[0], 4);
	
	texture1.sample(uv(1.0_r, 1.0_r), &pixel);
	EXPECT_EQ(pixel[0], 1);

	texture1.sample(uv(-0.1_r, 0.0_r), &pixel);
	EXPECT_EQ(pixel[0], 3);

	texture1.sample(uv(-0.1_r, -0.1_r), &pixel);
	EXPECT_EQ(pixel[0], 4);

	// trial 2

	Frame frame2(2, 1);
	frame2.setPixel(0, 0, Frame::Pixel(4));
	frame2.setPixel(1, 0, Frame::Pixel(8));

	TNearestPixelTex2D<int, 1> texture2(frame2);
	texture2.setWrapMode(ETexWrapMode::REPEAT);

	texture2.sample(uv(0.25_r, 0.5_r), &pixel);
	EXPECT_EQ(pixel[0], 4);

	texture2.sample(uv(0.75_r, 0.5_r), &pixel);
	EXPECT_EQ(pixel[0], 8);

	// trial 3

	Frame frame3(1, 2);
	frame3.setPixel(0, 0, Frame::Pixel(6));
	frame3.setPixel(0, 1, Frame::Pixel(12));

	TNearestPixelTex2D<int, 1> texture3(frame3);
	texture3.setWrapMode(ETexWrapMode::REPEAT);

	texture3.sample(uv(0.5_r, 0.25_r), &pixel);
	EXPECT_EQ(pixel[0], 6);

	texture3.sample(uv(0.5_r, 0.75_r), &pixel);
	EXPECT_EQ(pixel[0], 12);
}

TEST(PixelBasedTextureTest, BilinearFilteredTexture)
{
	auto uv = [](const real u, const real v)
	{
		return SampleLocation(Vector3R(u, v, 0), math::EColorUsage::RAW);
	};

	using Frame = TFrame<float, 1>;

	Frame frame1(2, 2);
	frame1.setPixel(0, 0, Frame::Pixel(1.0f));
	frame1.setPixel(0, 1, Frame::Pixel(2.0f));
	frame1.setPixel(1, 0, Frame::Pixel(3.0f));
	frame1.setPixel(1, 1, Frame::Pixel(4.0f));

	TBilinearPixelTex2D<float, 1> texture1(frame1);
	texture1.setWrapMode(ETexWrapMode::CLAMP_TO_EDGE);
	
	TTexPixel<float, 1> pixel;

	texture1.sample(uv(0.25_r, 0.25_r), &pixel);
	EXPECT_FLOAT_EQ(pixel[0], 1.0f);

	texture1.sample(uv(0.25_r, 0.75_r), &pixel);
	EXPECT_FLOAT_EQ(pixel[0], 2.0f);

	texture1.sample(uv(0.75_r, 0.25_r), &pixel);
	EXPECT_FLOAT_EQ(pixel[0], 3.0f);

	texture1.sample(uv(0.75_r, 0.75_r), &pixel);
	EXPECT_FLOAT_EQ(pixel[0], 4.0f);

	texture1.sample(uv(0.5_r, 0.5_r), &pixel);
	EXPECT_FLOAT_EQ(pixel[0], (1.0f + 2.0f + 3.0f + 4.0f) / 4.0f);
}
