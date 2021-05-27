#include "util.h"

#include <DataIO/SDL/Introspect/TSdlMethod.h>
#include <DataIO/SDL/ISdlResource.h>

#include <gtest/gtest.h>

#include <string>
#include <cstddef>

using namespace ph;

namespace
{
	class TestResource : public ISdlResource
	{
	public:
		int testInt;
	};

	struct TestMethodStruct
	{
		std::string str;

		void operator () (TestResource& res)
		{
			str = "hello";
		}
	};
}

TEST(TSdlMethodTest, DefaultStates)
{
	{
		TSdlMethod<TestMethodStruct, TestResource> method("testMethod");

		PH_EXPECT_STRING_EQ(method.getName(), "testMethod");
		PH_EXPECT_STRING_EQ(method.getDescription(), "");
		EXPECT_EQ(method.numParams(), 0);
		
		// Getting out-of-bound parameters is allowed
		for(std::size_t i = 0; i < 1000; ++i)
		{
			EXPECT_TRUE(method.getParam(i) == nullptr);
		}
	}
}
