#include "util.h"

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
		PH_EXPECT_REAL_EQ(sdlReal.getDefaultValue(), 0.0_r);

		RealOwner owner;
		owner.value = 3;

		SdlInputContext ctx;

		EXPECT_NO_THROW(sdlReal.fromSdl(owner, "123.456", ctx));
		PH_EXPECT_REAL_EQ(owner.value, 123.456_r);

		// Value with spaces
		EXPECT_NO_THROW(sdlReal.fromSdl(owner, " 12.5  ", ctx));
		PH_EXPECT_REAL_EQ(owner.value, 12.5_r);

		// TODO: scientific notation

		// Fallback to default for optional & nice-to-have fields

		sdlReal.defaultTo(777.0f);
		sdlReal.withImportance(EFieldImportance::OPTIONAL);
		EXPECT_NO_THROW(sdlReal.fromSdl(owner, "yoyoyo", ctx));
		PH_EXPECT_REAL_EQ(owner.value, 777.0_r);

		sdlReal.defaultTo(888.0f);
		sdlReal.withImportance(EFieldImportance::NICE_TO_HAVE);
		EXPECT_NO_THROW(sdlReal.fromSdl(owner, "test", ctx));
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

		EXPECT_THROW(sdlReal.fromSdl(owner, "fltflt", ctx), SdlLoadError);
		PH_EXPECT_REAL_EQ(owner.value, 6.8_r);// owner value should not update
	}
}
