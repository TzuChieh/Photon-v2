#include <SDL/TSdl.h>
#include <Actor/Actor.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(TSdlTest, Concepts)
{
	static_assert(TSdl<Actor>::getCategory() != ESdlTypeCategory::Unspecified);
}
