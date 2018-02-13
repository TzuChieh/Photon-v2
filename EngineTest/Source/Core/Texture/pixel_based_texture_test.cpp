#include <Core/Texture/TNearestPixelTex2D.h>

#include <gtest/gtest.h>

TEST(PixelBasedTextureTest, NearestFilteredTexture)
{
	using namespace ph;

	typedef TFrame<int, 1> Frame;

	Frame frame(2, 2);
	frame.setPixel(0, 0, Frame::Pixel(1));
	frame.setPixel(0, 1, Frame::Pixel(2));
	frame.setPixel(1, 0, Frame::Pixel(3));
	frame.setPixel(1, 1, Frame::Pixel(4));

	TNearestPixelTex2D<int, 1> texture(frame);

	auto uv = [](const real u, const real v)
	{
		return SampleLocation(SurfaceHit(), Vector3R(u, v, 0));
	};

	TTexPixel<int, 1> pixel;

	texture.sample(uv(0.0_r, 0.0_r), &pixel);
	EXPECT_EQ(pixel[0], 1);

	texture.sample(uv(0.4_r, 0.4_r), &pixel);
	EXPECT_EQ(pixel[0], 1);

	texture.sample(uv(0.6_r, 0.4_r), &pixel);
	EXPECT_EQ(pixel[0], 3);

	texture.sample(uv(0.2_r, 0.8_r), &pixel);
	EXPECT_EQ(pixel[0], 2);

	texture.sample(uv(0.9_r, 0.9_r), &pixel);
	EXPECT_EQ(pixel[0], 4);
	
	texture.sample(uv(1.0_r, 1.0_r), &pixel);
	EXPECT_EQ(pixel[0], 1);

	texture.sample(uv(-0.1_r, 0.0_r), &pixel);
	EXPECT_EQ(pixel[0], 3);

	texture.sample(uv(-0.1_r, -0.1_r), &pixel);
	EXPECT_EQ(pixel[0], 4);
}
