#include <DataIO/SDL/Introspect/TSdlString.h>

#include <gtest/gtest.h>

#include <string>
#include <type_traits>

using namespace ph;

namespace
{
	class StrOwner
	{
	public:
		std::string s;
	};
}

TEST(TSdlStringTest, RequiredProperties)
{
	{
		using FieldType = TSdlString<StrOwner>;
		EXPECT_TRUE(std::is_copy_constructible_v<FieldType>);
		EXPECT_TRUE(std::is_move_constructible_v<FieldType>);
	}
}

TEST(TSdlStringTest, ReadFromSdl)
{
	// Note that reading a string can hardly fail--basically it just stores
	// the SDL value provided.

	{
		TSdlString<StrOwner> sdlStr("dog-name", &StrOwner::s);

		// By default the default value is empty string
		EXPECT_EQ(sdlStr.defaultValue(), "");

		sdlStr.defaultTo("Lucy");
		EXPECT_EQ(sdlStr.defaultValue(), "Lucy");

		StrOwner owner;
		owner.s = "this is some random string";

		SdlInputContext ctx;

		EXPECT_NO_THROW(sdlStr.fromSdl(owner, SdlPayload("Jason the Dog"), ctx));
		EXPECT_EQ(owner.s, "Jason the Dog");

		// Should not remove any spaces from string
		EXPECT_NO_THROW(sdlStr.fromSdl(owner, SdlPayload("  2-space prefix & 3-space suffix   "), ctx));
		EXPECT_EQ(owner.s, "  2-space prefix & 3-space suffix   ");

		// Read string with different importance

		sdlStr.optional();
		EXPECT_NO_THROW(sdlStr.fromSdl(owner, SdlPayload("WhaT"), ctx));
		EXPECT_EQ(owner.s, "WhaT");

		sdlStr.niceToHave();
		EXPECT_NO_THROW(sdlStr.fromSdl(owner, SdlPayload("hmm?"), ctx));
		EXPECT_EQ(owner.s, "hmm?");

		sdlStr.required();
		EXPECT_NO_THROW(sdlStr.fromSdl(owner, SdlPayload(" testing"), ctx));
		EXPECT_EQ(owner.s, " testing");
	}
}