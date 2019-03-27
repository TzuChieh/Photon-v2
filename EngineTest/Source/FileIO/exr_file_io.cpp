#include "config.h"

#include <FileIO/ExrFileReader.h>

#include <gtest/gtest.h>

#include <cmath>

using namespace ph;

TEST(ExrFileIOTest, ReadSimpleRgbFiles)
{
	// trial 1:
	{
		ph::ExrFileReader reader(ph::Path(PH_TEST_RESOURCE_PATH("EXR/2x1_black_white.exr")));

		HdrRgbFrame frame;
		EXPECT_TRUE(reader.load(&frame));

		EXPECT_EQ(frame.widthPx(),  2);
		EXPECT_EQ(frame.heightPx(), 1);

		HdrRgbFrame::Pixel pixel;

		// black
		frame.getPixel(0, 0, &pixel);
		EXPECT_FLOAT_EQ(pixel[0], 0);
		EXPECT_FLOAT_EQ(pixel[1], 0);
		EXPECT_FLOAT_EQ(pixel[2], 0);

		// white
		frame.getPixel(1, 0, &pixel);
		EXPECT_FLOAT_EQ(pixel[0], 1);
		EXPECT_FLOAT_EQ(pixel[1], 1);
		EXPECT_FLOAT_EQ(pixel[2], 1);
	}
	
	// trial 2:
	{
		ph::ExrFileReader reader(ph::Path(PH_TEST_RESOURCE_PATH("EXR/2x2_B,(50,100,150),R,G.exr")));

		HdrRgbFrame frame;
		EXPECT_TRUE(reader.load(&frame));

		EXPECT_EQ(frame.widthPx(),  2);
		EXPECT_EQ(frame.heightPx(), 2);

		HdrRgbFrame::Pixel pixel;

		// blue
		frame.getPixel(0, 0, &pixel);
		EXPECT_FLOAT_EQ(pixel[0], 0);
		EXPECT_FLOAT_EQ(pixel[1], 0);
		EXPECT_FLOAT_EQ(pixel[2], 1);

		// (50, 100, 150)
		// here we check abs. error since pixel value was in half floating 
		// point format which has less precision
		frame.getPixel(1, 0, &pixel);
		EXPECT_LT(std::abs(pixel[0] - 50.0f / 255.0f),  0.0002f);
		EXPECT_LT(std::abs(pixel[1] - 100.0f / 255.0f), 0.0002f);
		EXPECT_LT(std::abs(pixel[2] - 150.0f / 255.0f), 0.0002f);

		// red
		frame.getPixel(0, 1, &pixel);
		EXPECT_FLOAT_EQ(pixel[0], 1);
		EXPECT_FLOAT_EQ(pixel[1], 0);
		EXPECT_FLOAT_EQ(pixel[2], 0);

		// green
		frame.getPixel(1, 1, &pixel);
		EXPECT_FLOAT_EQ(pixel[0], 0);
		EXPECT_FLOAT_EQ(pixel[1], 1);
		EXPECT_FLOAT_EQ(pixel[2], 0);
	}
}