#include <Engine/DataIO/EXR/ExrFile.h>
#include <Engine/DataIO/FileSystem/Filesystem.h>
#include <Engine/DataIO/FileSystem/TProjectPath.h>
#include <Engine/DataIO/io_utils.h>
#include <Engine/Frame/PictureData.h>
#include <Engine/Frame/RegularPicture.h>
#include <Engine/Frame/TFrame.h>

#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <string_view>

using namespace ph;

TEST(ExrFileIOTest, ReadSimpleRgbFiles)
{
	constexpr std::array<std::string_view, 3> channelNames = {"R", "G", "B"};

	// Trial 1:
	{
		ExrFile file(EngineTestResourcePath("EXR/2x1_black_white.exr"));
		const PictureData pictureData = file.load(channelNames);
		const HdrRgbFrame frame = pictureData.toFrame<HdrComponent, 3>();

		EXPECT_EQ(frame.widthPx(),  2);
		EXPECT_EQ(frame.heightPx(), 1);

		HdrRgbFrame::PixelType pixel;

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
	
	// Trial 2:
	{
		ExrFile file(EngineTestResourcePath("EXR/2x2_B,(50,100,150),R,G.exr"));
		const PictureData pictureData = file.load(channelNames);
		const HdrRgbFrame frame = pictureData.toFrame<HdrComponent, 3>();

		EXPECT_EQ(frame.widthPx(),  2);
		EXPECT_EQ(frame.heightPx(), 2);

		HdrRgbFrame::PixelType pixel;

		// blue
		frame.getPixel(0, 0, &pixel);
		EXPECT_FLOAT_EQ(pixel[0], 0);
		EXPECT_FLOAT_EQ(pixel[1], 0);
		EXPECT_FLOAT_EQ(pixel[2], 1);

		// (50, 100, 150)
		// Use an absolute tolerance because the source uses half precision, which has less precision.
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

TEST(ExrFileIOTest, SimpleStandaloneYFiles)
{
	const std::array<float32, 2> yComponents = {0.25f, 0.75f};
	const PictureData yPictureData(
		math::Vector2S(2, 1),
		1,
		EPicturePixelComponent::Float32,
		yComponents.data(),
		yComponents.size());
	const Path exrDir = EngineTestIntermediatePath(
		"ExrFileIOTest/SimpleStandaloneYFiles");
	const Path exrFile = exrDir / "roughness.exr";
	Filesystem::remove(exrDir, true);
	Filesystem::createDirectories(exrDir);
	ExrFile::save(yPictureData, exrFile, {"Y"});

	const RegularPicture picture = io_utils::load_picture(exrFile);
	EXPECT_EQ(picture.getWidthPx(), 2);
	EXPECT_EQ(picture.getHeightPx(), 1);
	ASSERT_EQ(picture.numComponents(), 1);
	EXPECT_EQ(picture.getComponentType(), EPicturePixelComponent::Float32);
	EXPECT_TRUE(picture.getFormat().isGrayscale());

	const auto components = picture.getPixels().getComponents<HdrComponent>();
	ASSERT_EQ(components.size(), 2);
	EXPECT_FLOAT_EQ(components[0], 0.25f);
	EXPECT_FLOAT_EQ(components[1], 0.75f);
}

//TEST(ExrFileIOTest, WriteSimpleRgbFiles)
//{
//	using namespace ph;
//	using Pixel = HdrRgbFrame::Pixel;
//
//	ExrFileWriter writer(Path("./test.exr"));
//
//	HdrRgbFrame frame(3, 2);
//	frame.setPixel(0, 0, Pixel({0, 0, 0}));
//	frame.setPixel(1, 0, Pixel({1, 0, 0}));
//	frame.setPixel(2, 0, Pixel({0, 1, 0}));
//	frame.setPixel(0, 1, Pixel({0, 0, 1}));
//	frame.setPixel(1, 1, Pixel({1, 1, 0}));
//	frame.setPixel(2, 1, Pixel({1, 1, 1}));
//
//	EXPECT_TRUE(writer.save(frame));
//}
