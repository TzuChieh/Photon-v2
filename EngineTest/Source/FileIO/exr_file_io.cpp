#include <FileIO/ExrFileReader.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(ExrFileIOTest, ReadFileInfo)
{
	ph::ExrFileReader reader(ph::Path("./2x1_black_white.exr"));
	HdrRgbFrame frame;
	reader.load(&frame);
}