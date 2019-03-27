#include "config.h"

#include <FileIO/ExrFileReader.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(ExrFileIOTest, ReadSimpleRgbFiles)
{
	ph::ExrFileReader reader1(ph::Path(PH_TEST_RESOURCE_PATH("EXR/2x1_black_white.exr")));
	HdrRgbFrame frame1;
	EXPECT_TRUE(reader1.load(&frame1));
}