#include "util.h"

#include <DataIO/SDL/Introspect/TSdlReal.h>

#include <gtest/gtest.h>

#include <type_traits>

using namespace ph;

namespace
{
	class RealOwner
	{
	public:
		real value;
	};
}

TEST(TSdlReadTest, RequiredProperties)
{
	{
		using FieldType = TSdlReal<RealOwner, real>;
		EXPECT_TRUE(std::is_copy_constructible_v<FieldType>);
		EXPECT_TRUE(std::is_move_constructible_v<FieldType>);
	}
}

TEST(TSdlReadTest, ReadFromSdl)
{
	{
		TSdlReal<RealOwner, real> sdlReal("number", &RealOwner::value);

		// Zero initialization performed on default value by default
		PH_EXPECT_REAL_EQ(sdlReal.defaultValue(), 0.0_r);

		RealOwner owner;
		owner.value = 3;

		SdlInputContext ctx;

		EXPECT_NO_THROW(sdlReal.fromSdl(owner, SdlInputPayload("123.456"), ctx));
		PH_EXPECT_REAL_EQ(owner.value, 123.456_r);

		// Value with spaces
		EXPECT_NO_THROW(sdlReal.fromSdl(owner, SdlInputPayload(" 12.5  "), ctx));
		PH_EXPECT_REAL_EQ(owner.value, 12.5_r);

		// TODO: scientific notation

		// Fallback to default for optional & nice-to-have fields

		sdlReal.defaultTo(777.0f);
		sdlReal.optional();
		EXPECT_NO_THROW(sdlReal.fromSdl(owner, SdlInputPayload("yoyoyo"), ctx));
		PH_EXPECT_REAL_EQ(owner.value, 777.0_r);

		sdlReal.defaultTo(888.0f);
		sdlReal.niceToHave();
		EXPECT_NO_THROW(sdlReal.fromSdl(owner, SdlInputPayload("test"), ctx));
		PH_EXPECT_REAL_EQ(owner.value, 888.0_r);
	}
	
	// Disable fallback for fields
	{
		TSdlReal<RealOwner, real> sdlReal("number", &RealOwner::value);
		sdlReal.enableFallback(false);
		sdlReal.defaultTo(123.0_r);

		RealOwner owner;
		owner.value = 6.8_r;

		SdlInputContext ctx;

		EXPECT_THROW(sdlReal.fromSdl(owner, SdlInputPayload("fltflt"), ctx), SdlLoadError);
		PH_EXPECT_REAL_EQ(owner.value, 6.8_r);// owner value should not update
	}
}
