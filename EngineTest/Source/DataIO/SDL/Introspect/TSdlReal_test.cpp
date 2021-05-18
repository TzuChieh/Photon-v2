#include <DataIO/SDL/Introspect/TSdlReal.h>

#include <gtest/gtest.h>

using namespace ph;

namespace
{
	class RealOwner
	{
	public:
		real value;
	};
}

TEST(TSdlReadTest, ReadFromSdl)
{
	{
		TSdlReal<RealOwner, real> sdlReal("number", &RealOwner::value);

		// Zero initialization performed on default value by default
		EXPECT_EQ(sdlReal.getDefaultValue(), 0.0f);

		RealOwner owner;
		owner.value = 3;

		SdlInputContext ctx;

		EXPECT_NO_THROW(sdlReal.fromSdl(owner, "123.456", ctx));
		EXPECT_FLOAT_EQ(owner.value, 123.456f);

		// Value with spaces
		EXPECT_NO_THROW(sdlReal.fromSdl(owner, " 12.5  ", ctx));
		EXPECT_FLOAT_EQ(owner.value, 12.5f);

		// TODO: scientific notation

		// Fallback to default for optional & nice-to-have fields

		sdlReal.defaultTo(777.0f);
		sdlReal.withImportance(EFieldImportance::OPTIONAL);
		EXPECT_NO_THROW(sdlReal.fromSdl(owner, "yoyoyo", ctx));
		EXPECT_FLOAT_EQ(owner.value, 777.0f);

		sdlReal.defaultTo(888.0f);
		sdlReal.withImportance(EFieldImportance::NICE_TO_HAVE);
		EXPECT_NO_THROW(sdlReal.fromSdl(owner, "test", ctx));
		EXPECT_FLOAT_EQ(owner.value, 888.0f);
	}

	// Disable fallback for fields
	{
		TSdlReal<RealOwner, real> sdlReal("number", &RealOwner::value);
		sdlReal.enableFallback(false);
		sdlReal.defaultTo(123.0f);

		RealOwner owner;
		owner.value = 6.8f;

		SdlInputContext ctx;

		EXPECT_THROW(sdlReal.fromSdl(owner, "fltflt", ctx), SdlLoadError);
		EXPECT_NE(owner.value, 123.0f);// owner value should not update
	}
}
