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
