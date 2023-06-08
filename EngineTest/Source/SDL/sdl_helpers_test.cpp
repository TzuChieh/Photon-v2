#include <SDL/sdl_helpers.h>
#include <Actor/Actor.h>

#include <gtest/gtest.h>

using namespace ph;

namespace
{

struct DummyStruct
{};

}

TEST(SdlHelpersTest, Concepts)
{
	static_assert(CSdlResource<DummyStruct> == false);
	static_assert(CSdlResource<Actor> == true);

	static_assert(CHasStaticSdlCategoryInfo<DummyStruct> == false);
	static_assert(CHasStaticSdlCategoryInfo<Actor> == true);

	static_assert(sdl::category_of<DummyStruct>() == ESdlTypeCategory::Unspecified);
	static_assert(sdl::category_of<Actor>() != ESdlTypeCategory::Unspecified);
}

TEST(SdlHelpersTest, IsBundledResourceIdentifier)
{
	EXPECT_TRUE(sdl::is_bundled_resource_identifier("//some/location"));
	EXPECT_TRUE(sdl::is_bundled_resource_identifier("//other/"));
	EXPECT_TRUE(sdl::is_bundled_resource_identifier(" //other/with/spaces"));
	EXPECT_TRUE(sdl::is_bundled_resource_identifier("   //other/with/more/  spaces "));
	EXPECT_FALSE(sdl::is_bundled_resource_identifier("some/location"));
	EXPECT_FALSE(sdl::is_bundled_resource_identifier("other"));
	EXPECT_FALSE(sdl::is_bundled_resource_identifier("/other/aaa.dat"));
	EXPECT_FALSE(sdl::is_bundled_resource_identifier("./other/aaa.dat"));
	EXPECT_FALSE(sdl::is_bundled_resource_identifier("C:\\other\\aaa.dat"));
	EXPECT_FALSE(sdl::is_bundled_resource_identifier(".//other/bbb.jpg"));
}
