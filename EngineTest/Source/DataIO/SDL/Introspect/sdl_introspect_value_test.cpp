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
		ASSERT_TRUE(sdlReal.fromSdl(owner, "yoyoyo", std::string()));
		EXPECT_FLOAT_EQ(owner.value, 777.0f);

		sdlReal.defaultTo(888.0f);
		sdlReal.withImportance(EFieldImportance::NICE_TO_HAVE);
		ASSERT_TRUE(sdlReal.fromSdl(owner, "test", std::string()));
		EXPECT_FLOAT_EQ(owner.value, 888.0f);

		// Default value fallback is not applicable for required fields
		sdlReal.withImportance(EFieldImportance::REQUIRED);
		sdlReal.defaultTo(123.0f);
		ASSERT_FALSE(sdlReal.fromSdl(owner, "hello", std::string()));
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

		ASSERT_TRUE(sdlInt.fromSdl(owner, "123", std::string()));
		EXPECT_EQ(owner.value, 123);

		ASSERT_TRUE(sdlInt.fromSdl(owner, "-7", std::string()));
		EXPECT_EQ(owner.value, -7);

		// Fallback to default for optional & nice-to-have fields

		sdlInt.defaultTo(12);
		sdlInt.withImportance(EFieldImportance::OPTIONAL);
		ASSERT_TRUE(sdlInt.fromSdl(owner, "what", std::string()));
		EXPECT_EQ(owner.value, 12);

		sdlInt.defaultTo(333);
		sdlInt.withImportance(EFieldImportance::NICE_TO_HAVE);
		ASSERT_TRUE(sdlInt.fromSdl(owner, "not an int", std::string()));
		EXPECT_EQ(owner.value, 333);

		// Default value fallback is not applicable for required fields
		sdlInt.withImportance(EFieldImportance::REQUIRED);
		sdlInt.defaultTo(22);
		ASSERT_FALSE(sdlInt.fromSdl(owner, "?121??", std::string()));
		EXPECT_NE(owner.value, 22);// owner value not updated
	}
}
