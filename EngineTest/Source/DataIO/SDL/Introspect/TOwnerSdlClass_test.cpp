#include "util.h"

#include <DataIO/SDL/Introspect/TOwnerSdlClass.h>
#include <DataIO/SDL/ISdlResource.h>
#include <Common/primitive_type.h>

#include <gtest/gtest.h>

#include <string>
#include <cstddef>

using namespace ph;

namespace
{
	class TestResource : public ISdlResource
	{
	public:
		real r;
		std::string str;
	};
}

TEST(TOwnerSdlClassTest, DefaultStates)
{
	{
		TOwnerSdlClass<TestResource> sdlClass("testCat", "testName");

		EXPECT_EQ(sdlClass.numFields(), 0);
		EXPECT_EQ(sdlClass.numFunctions(), 0);
		EXPECT_STREQ(sdlClass.getCategory().c_str(), "testCat");
		EXPECT_STREQ(sdlClass.getTypeName().c_str(), "testName");
		EXPECT_STREQ(sdlClass.getDescription().c_str(), "");

		EXPECT_TRUE(sdlClass.getBase() == nullptr);
		EXPECT_FALSE(sdlClass.isDerived());
		
		// Getting out-of-bound field & function are allowed
		for(std::size_t i = 0; i < 1000; ++i)
		{
			EXPECT_TRUE(sdlClass.getField(i) == nullptr);
			EXPECT_TRUE(sdlClass.getFunction(i) == nullptr);
			EXPECT_TRUE(sdlClass.getOwnedField(i) == nullptr);
		}
	}
}
