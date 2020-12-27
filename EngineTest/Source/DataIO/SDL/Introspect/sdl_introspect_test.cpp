#include <DataIO/SDL/Introspect/TSdlReal.h>

#include <gtest/gtest.h>

using namespace ph;

namespace
{
	class SimpleRealOwner
	{
	public:
		real num;
	};
}

TEST(SdlIntrospectTest, RealFromSdl)
{
	{
		TSdlReal<SimpleRealOwner, real> sdlReal("number", &SimpleRealOwner::num);

		SimpleRealOwner owner;
		owner.num = 3;

		ASSERT_TRUE(sdlReal.fromSdl(owner, "123.456", std::string()));
		EXPECT_FLOAT_EQ(owner.num, 123.456f);

		// Fallback to default for optional & nice-to-have fields

		sdlReal.defaultTo(777.0f);
		sdlReal.withImportance(EFieldImportance::OPTIONAL);
		ASSERT_TRUE(sdlReal.fromSdl(owner, "yoyoyo", std::string()));
		EXPECT_FLOAT_EQ(owner.num, 777.0f);

		sdlReal.defaultTo(888.0f);
		sdlReal.withImportance(EFieldImportance::NICE_TO_HAVE);
		ASSERT_TRUE(sdlReal.fromSdl(owner, "test", std::string()));
		EXPECT_FLOAT_EQ(owner.num, 888.0f);

		// Default value fallback is not applicable for required fields
		sdlReal.withImportance(EFieldImportance::REQUIRED);
		sdlReal.defaultTo(123.0f);
		ASSERT_FALSE(sdlReal.fromSdl(owner, "hello", std::string()));
		EXPECT_NE(owner.num, 123.0f);
	}
}
