#include <Frame/_mipmap_gen.h>

#include <gtest/gtest.h>

#include <cmath>

TEST(FrameProcessorTest, GenerateMipmaps)
{
	// checks whether a frame has the same value <value> in each pixel component
	auto isConstant = [](const ph::HdrRgbFrame& frame, const ph::real value) -> bool
	{
		for(ph::uint32 y = 0; y < frame.heightPx(); ++y)
		{
			for(ph::uint32 x = 0; x < frame.widthPx(); ++x)
			{
				ph::HdrRgbFrame::Pixel pixel;
				frame.getPixel(x, y, &pixel);
				for(std::size_t i = 0; i < pixel.size(); ++i)
				{
					if(std::abs(pixel[i] - value) > static_cast<ph::real>(0.00001))
					{
						return false;
					}
				}
			}
		}
		return true;
	};

	ph::mipmapgen processor(3);
	ph::HdrRgbFrame src(127, 127);
	src.fill(7);

	// 2^6 < size-of-src < 2^7, size-of-src will be rounded up to 2^7 (128)
	auto futureMipmaps = processor.genMipmaps(src);

	auto mipmaps = futureMipmaps.get();

	EXPECT_EQ(mipmaps.size(), 8);
	for(std::size_t i = 0; i < mipmaps.size(); ++i)
	{
		EXPECT_TRUE(mipmaps[i].widthPx() == mipmaps[i].heightPx());
		EXPECT_EQ  (mipmaps[i].widthPx(), 128 >> i);
		EXPECT_TRUE(isConstant(mipmaps[i], 7));
	}
}