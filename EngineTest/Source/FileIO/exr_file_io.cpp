#include "config.h"

#include <FileIO/ExrFileReader.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(ExrFileIOTest, ReadFileInfo)
{
	ph::ExrFileReader reader(ph::Path(PH_TEST_RESOURCE_PATH("EXR/2x1_black_white.exr")));
	HdrRgbFrame frame;
	reader.load(&frame);
}