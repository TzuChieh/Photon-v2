#include <Frame/TFrame.h>
#include <Math/Function/TConstant2D.h>

#include <gtest/gtest.h>

TEST(TFrameTest, CorrectAttributes)
{
	ph::TFrame<int, 5> frame1(12, 34);

	EXPECT_EQ(frame1.widthPx(),  12);
	EXPECT_EQ(frame1.heightPx(), 34);
	EXPECT_FALSE(frame1.isEmpty());

	ph::TFrame<int, 5> frame2(1, 0);

	EXPECT_TRUE(frame2.isEmpty());
}

TEST(TFrameTest, FillWithSpecificValue)
{
	ph::TFrame<float, 2> frame(10, 10);
	frame.fill(-3.3f);

	for(ph::uint32 y = 0; y < frame.heightPx(); ++y)
	{
		for(ph::uint32 x = 0; x < frame.widthPx(); ++x)
		{
			ph::TFrame<float, 2>::Pixel pixel;
			frame.getPixel(x, y, &pixel);

			EXPECT_EQ(pixel[0], -3.3f);
			EXPECT_EQ(pixel[1], -3.3f);
		}
	}
}

TEST(TFrameTest, GenerateMonochromaticPixel)
{
	const auto& pixel = ph::TFrame<float, 3>::getMonochromaticPixel(1.2f);

	EXPECT_EQ(pixel[0], 1.2f);
	EXPECT_EQ(pixel[1], 1.2f);
	EXPECT_EQ(pixel[2], 1.2f);
}

TEST(TFrameTest, Sampling)
{
	typedef ph::TFrame<float, 5> Frame;

	Frame src(100, 200);
	Frame dst(20, 20);
	
	ph::TConstant2D<ph::float64> kernel(1.0);

	// averaging nearby pixels
	src.fill(7.0f);
	src.sample(dst, kernel, 2);
	for(ph::uint32 y = 0; y < dst.heightPx(); ++y)
	{
		for(ph::uint32 x = 0; x < dst.widthPx(); ++x)
		{
			Frame::Pixel pixel;
			dst.getPixel(x, y, &pixel);
			for(std::size_t i = 0; i < 5; ++i)
			{
				EXPECT_FLOAT_EQ(pixel[i], 7.0f);
			}
		}
	}

	const auto& one = Frame::getMonochromaticPixel(1.0f);
	const auto& two = Frame::getMonochromaticPixel(2.0f);
	for(ph::uint32 y = 0; y < src.heightPx(); ++y)
	{
		for(ph::uint32 x = 0; x < src.widthPx(); ++x)
		{
			if(x < src.widthPx() / 2)
			{
				src.setPixel(x, y, one);
			}
			else
			{
				src.setPixel(x, y, two);
			}
		}
	}

	// averaging all pixels
	src.sample(dst, kernel, 9999);
	for(ph::uint32 y = 0; y < dst.heightPx(); ++y)
	{
		for(ph::uint32 x = 0; x < dst.widthPx(); ++x)
		{
			Frame::Pixel pixel;
			dst.getPixel(x, y, &pixel);
			for(std::size_t i = 0; i < 5; ++i)
			{
				EXPECT_FLOAT_EQ(pixel[i], 1.5f);
			}
		}
	}
}