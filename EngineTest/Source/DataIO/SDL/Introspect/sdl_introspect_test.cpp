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
	}
}
