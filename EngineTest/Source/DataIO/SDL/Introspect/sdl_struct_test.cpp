#include <DataIO/SDL/Introspect/TOwnerSdlStruct.h>

#include <gtest/gtest.h>

using namespace ph;

namespace
{
	struct EmptyStruct
	{};
}

TEST(SdlStructTest, DefaultStates)
{
	{
		TOwnerSdlStruct<EmptyStruct> sdlStruct("test-struct");
		EXPECT_EQ(sdlStruct.numFields(), 0);
		EXPECT_EQ(sdlStruct.getFields().numFields(), 0);

		// Getting non-existent fields is allowed and the result will be nullptr
		for(std::size_t fieldIdx = 0; fieldIdx < 1024; ++fieldIdx)
		{
			EXPECT_TRUE(sdlStruct.getField(fieldIdx) == nullptr);
			EXPECT_TRUE(sdlStruct.getFields().getField(fieldIdx) == nullptr);
		}
	}
}

// TODO: add field and add struct
