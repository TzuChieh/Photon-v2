#include <DataIO/SDL/sdl_helpers.h>
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
	static_assert(CIsSdlResource<DummyStruct> == false);
	static_assert(CIsSdlResource<Actor> == true);

	static_assert(CHasStaticSdlCategoryInfo<DummyStruct> == false);
	static_assert(CHasStaticSdlCategoryInfo<Actor> == true);

	static_assert(sdl::category_of<DummyStruct>() == ETypeCategory::UNSPECIFIED);
	static_assert(sdl::category_of<Actor>() != ETypeCategory::UNSPECIFIED);
}

TEST(SdlHelpersTest, IsResourceIdentifier)
{
	EXPECT_TRUE(sdl::is_resource_identifier("/some/location"));
	EXPECT_TRUE(sdl::is_resource_identifier("/other/"));
	EXPECT_FALSE(sdl::is_resource_identifier("some/location"));
	EXPECT_FALSE(sdl::is_resource_identifier("other"));
}
