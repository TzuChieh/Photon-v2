#include <DataIO/SDL/Introspect/TSdlReal.h>
#include <DataIO/SDL/Introspect/TSdlInteger.h>

#include <gtest/gtest.h>

using namespace ph;

namespace
{
	template<typename Value>
	class TSimpleValueOwner
	{
	public:
		Value value;
	};
}

TEST(SdlIntrospectValueTest, RealFromSdl)
{
	using SimpleRealOwner = TSimpleValueOwner<real>;

	{
		TSdlReal<SimpleRealOwner, real> sdlReal("number", &SimpleRealOwner::value);

		// Zero initialization performed on default value by default
		EXPECT_EQ(sdlReal.getDefaultValue(), 0.0f);

		SimpleRealOwner owner;
		owner.value = 3;

		SdlInputContext ctx;

		EXPECT_NO_THROW(sdlReal.fromSdl(owner, "123.456", ctx));
		EXPECT_FLOAT_EQ(owner.value, 123.456f);

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
		TSdlReal<SimpleRealOwner, real> sdlReal("number", &SimpleRealOwner::value);
		sdlReal.enableFallback(false);
		sdlReal.defaultTo(123.0f);

		SimpleRealOwner owner;
		owner.value = 6.8f;

		SdlInputContext ctx;

		EXPECT_THROW(sdlReal.fromSdl(owner, "fltflt", ctx), SdlLoadError);
		EXPECT_NE(owner.value, 123.0f);// owner value not updated
	}
}

TEST(SdlIntrospectValueTest, IntegerFromSdl)
{
	using SimpleIntOwner = TSimpleValueOwner<integer>;

	{
		TSdlInteger<SimpleIntOwner, integer> sdlInt("number", &SimpleIntOwner::value);

		// Zero initialization performed on default value by default
		EXPECT_EQ(sdlInt.getDefaultValue(), 0);

		SimpleIntOwner owner;
		owner.value = 666;

		SdlInputContext ctx;

		EXPECT_NO_THROW(sdlInt.fromSdl(owner, "123", ctx));
		EXPECT_EQ(owner.value, 123);

		EXPECT_NO_THROW(sdlInt.fromSdl(owner, "-7", ctx));
		EXPECT_EQ(owner.value, -7);

		// Fallback to default for fields with different importance

		sdlInt.defaultTo(12);
		sdlInt.withImportance(EFieldImportance::OPTIONAL);
		EXPECT_NO_THROW(sdlInt.fromSdl(owner, "what", ctx));
		EXPECT_EQ(owner.value, 12);

		sdlInt.defaultTo(333);
		sdlInt.withImportance(EFieldImportance::NICE_TO_HAVE);
		EXPECT_NO_THROW(sdlInt.fromSdl(owner, "not an int", ctx));
		EXPECT_EQ(owner.value, 333);

		sdlInt.defaultTo(444);
		sdlInt.withImportance(EFieldImportance::REQUIRED);
		EXPECT_NO_THROW(sdlInt.fromSdl(owner, "testing", ctx));
		EXPECT_EQ(owner.value, 444);
	}

	// Disable fallback for fields
	{
		TSdlInteger<SimpleIntOwner, integer> sdlInt("number", &SimpleIntOwner::value);
		sdlInt.enableFallback(false);
		sdlInt.defaultTo(22);

		SimpleIntOwner owner;
		owner.value = 1;

		SdlInputContext ctx;

		EXPECT_THROW(sdlInt.fromSdl(owner, "hello", ctx), SdlLoadError);
		EXPECT_NE(owner.value, 22);// owner value not updated
	}
}
