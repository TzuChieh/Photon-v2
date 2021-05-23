#include <DataIO/SDL/sdl_helpers.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(SdlHelpersTest, IsResourceIdentifier)
{
	EXPECT_TRUE(sdl::is_resource_identifier("/some/location"));
	EXPECT_TRUE(sdl::is_resource_identifier("/other/"));
	EXPECT_FALSE(sdl::is_resource_identifier("some/location"));
	EXPECT_FALSE(sdl::is_resource_identifier("other"));
}
