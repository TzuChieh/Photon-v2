#include <Frame/TFrame.h>
#include <Math/Function/TConstant2D.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(TFrameTest, CorrectAttributes)
{
	ph::TFrame<int, 5> frame1(12, 34);

	EXPECT_EQ(frame1.widthPx(),  12);
	EXPECT_EQ(frame1.heightPx(), 34);
	EXPECT_FALSE(frame1.isEmpty());

	ph::TFrame<int, 5> frame2(1, 0);

	EXPECT_TRUE(frame2.isEmpty());
}

TEST(TFrameTest, GetAndSetPixel)
{
	using namespace ph;

	TFrame<int, 5> frame1(2, 4);
	frame1.fill(7);
	const auto pixel1 = frame1.getPixel({1, 1});
	EXPECT_EQ(pixel1, decltype(pixel1)(7));

	TFrame<int, 2> frame2(2, 5);
	frame2.setPixel({0, 1}, TFrame<int, 2>::Pixel({1, 2}));
	const auto pixel2 = frame2.getPixel({0, 1});
	EXPECT_EQ(pixel2, decltype(pixel2)({1, 2}));
}

TEST(TFrameTest, FillAllWithSpecificValue)
{
	using namespace ph;

	TFrame<float, 2> frame(10, 10);
	frame.fill(-3.3f);

	for(uint32 y = 0; y < frame.heightPx(); ++y)
	{
		for(uint32 x = 0; x < frame.widthPx(); ++x)
		{
			TFrame<float, 2>::Pixel pixel;
			frame.getPixel(x, y, &pixel);

			EXPECT_EQ(pixel[0], -3.3f);
			EXPECT_EQ(pixel[1], -3.3f);
		}
	}
}

TEST(TFrameTest, FillRegionWithSpecificValue)
{
	using namespace ph;

	math::TAABB2D<uint32> region = {{2, 3}, {6, 8}};

	TFrame<float, 2> frame(10, 10);
	frame.fill(0);
	frame.fill(1.2f, region);

	for(uint32 y = 0; y < frame.heightPx(); ++y)
	{
		for(uint32 x = 0; x < frame.widthPx(); ++x)
		{
			const auto pixel = frame.getPixel({x, y});
			if(region.isIntersectingRange({x, y}))
			{
				EXPECT_EQ(pixel[0], 1.2f);
				EXPECT_EQ(pixel[1], 1.2f);
			}
			else
			{
				EXPECT_EQ(pixel[0], 0.0f);
				EXPECT_EQ(pixel[1], 0.0f);
			}
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
	using Frame = ph::TFrame<float, 5>;

	Frame src(100, 200);
	Frame dst(20, 20);
	
	TConstant2D<float64> kernel(1.0);

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

TEST(TFrameTest, Flipping)
{
	using Frame = TFrame<int, 2>;

	Frame::Pixel pixel;

	// horizontal flip

	Frame frame1(2, 2);

	frame1.setPixel(0, 1, Frame::Pixel({0, 1})); frame1.setPixel(1, 1, Frame::Pixel({2, 3})); 
	frame1.setPixel(0, 0, Frame::Pixel({4, 5})); frame1.setPixel(1, 0, Frame::Pixel({6, 7}));

	frame1.flipHorizontally();

	frame1.getPixel(0, 0, &pixel);
	EXPECT_TRUE(pixel == Frame::Pixel({6, 7}));

	frame1.getPixel(1, 0, &pixel);
	EXPECT_TRUE(pixel == Frame::Pixel({4, 5}));

	frame1.getPixel(0, 1, &pixel);
	EXPECT_TRUE(pixel == Frame::Pixel({2, 3}));

	frame1.getPixel(1, 1, &pixel);
	EXPECT_TRUE(pixel == Frame::Pixel({0, 1}));

	// vertical flip

	Frame frame2(2, 2);

	frame2.setPixel(0, 1, Frame::Pixel({0, 1})); frame2.setPixel(1, 1, Frame::Pixel({2, 3}));
	frame2.setPixel(0, 0, Frame::Pixel({4, 5})); frame2.setPixel(1, 0, Frame::Pixel({6, 7}));

	frame2.flipVertically();

	frame2.getPixel(0, 0, &pixel);
	EXPECT_TRUE(pixel == Frame::Pixel({0, 1}));

	frame2.getPixel(1, 0, &pixel);
	EXPECT_TRUE(pixel == Frame::Pixel({2, 3}));

	frame2.getPixel(0, 1, &pixel);
	EXPECT_TRUE(pixel == Frame::Pixel({4, 5}));

	frame2.getPixel(1, 1, &pixel);
	EXPECT_TRUE(pixel == Frame::Pixel({6, 7}));
}

TEST(TFrameTest, ForEachOperation)
{
	using Float2Frame = TFrame<float, 2>;

	Float2Frame frame1(1, 2);
	frame1.setPixel(0, 0, Float2Frame::Pixel({1, 2}));
	frame1.setPixel(0, 1, Float2Frame::Pixel({3, 4}));

	frame1.forEachPixel([](const Float2Frame::Pixel& pixel)
	{
		return pixel * 2;
	});

	Float2Frame::Pixel pixel;

	frame1.getPixel(0, 0, &pixel);
	EXPECT_FLOAT_EQ(pixel[0], 2);
	EXPECT_FLOAT_EQ(pixel[1], 4);

	frame1.getPixel(0, 1, &pixel);
	EXPECT_FLOAT_EQ(pixel[0], 6);
	EXPECT_FLOAT_EQ(pixel[1], 8);
}
