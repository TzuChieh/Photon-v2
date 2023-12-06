#include <SDL/SdlResourceLocator.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(SdlResourceLocatorTest, IsRecognized)
{
	EXPECT_TRUE(SdlResourceLocator::isRecognized(":some/location"));
	EXPECT_TRUE(SdlResourceLocator::isRecognized("ext:some/location"));
	EXPECT_TRUE(SdlResourceLocator::isRecognized(":other/"));
	EXPECT_TRUE(SdlResourceLocator::isRecognized("ext:other/"));
	EXPECT_TRUE(SdlResourceLocator::isRecognized(" :other/with/spaces"));
	EXPECT_TRUE(SdlResourceLocator::isRecognized(" ext:other/with/spaces"));
	EXPECT_TRUE(SdlResourceLocator::isRecognized("   :other/with/more/  spaces "));
	EXPECT_TRUE(SdlResourceLocator::isRecognized("   ext:other/with/more/  spaces "));
	EXPECT_FALSE(SdlResourceLocator::isRecognized("some/location"));
	EXPECT_FALSE(SdlResourceLocator::isRecognized("other"));
	EXPECT_FALSE(SdlResourceLocator::isRecognized("/other/aaa.dat"));
	EXPECT_FALSE(SdlResourceLocator::isRecognized("./other/aaa.dat"));
	EXPECT_FALSE(SdlResourceLocator::isRecognized("C:\\other\\aaa.dat"));
	EXPECT_FALSE(SdlResourceLocator::isRecognized(".//other/bbb.jpg"));
	EXPECT_FALSE(SdlResourceLocator::isRecognized("ext other/bbb.jpg"));
	EXPECT_FALSE(SdlResourceLocator::isRecognized(" ext  other/bbb.jpg"));
}
