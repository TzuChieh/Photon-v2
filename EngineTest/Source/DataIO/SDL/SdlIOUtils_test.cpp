#include <DataIO/SDL/SdlIOUtils.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(SdlIOUtilsTest, IsResourceIdentifier)
{
	EXPECT_TRUE(SdlIOUtils::isResourceIdentifier("/some/location"));
	EXPECT_TRUE(SdlIOUtils::isResourceIdentifier("/other/"));
	EXPECT_FALSE(SdlIOUtils::isResourceIdentifier("some/location"));
	EXPECT_FALSE(SdlIOUtils::isResourceIdentifier("other"));
}
