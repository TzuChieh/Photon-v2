#include "engine_test_util.h"

#include <Engine/SDL/Introspect/TSdlOwnerMethod.h>
#include <Engine/SDL/TSdlResourceBase.h>
#include <Engine/SDL/Introspect/TSdlString.h>
#include <Engine/SDL/SdlInputClauses.h>
#include <Engine/SDL/sdl_interface.h>

#include <gtest/gtest.h>

#include <string>
#include <cstddef>

using namespace ph;

namespace
{

class TestResource : public TSdlResourceBase<ESdlTypeCategory::Ref_Option>
{
public:
	std::string str;

	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<TestResource>)
	{
		return ClassType("dummy");
	}
};

struct TestMethodStruct
{
	std::string str;

	void operator () (TestResource& res)
	{
		res.str = str;
	}
};

}

TEST(TSdlOwnerMethodTest, DefaultStates)
{
	{
		TSdlOwnerMethod<TestMethodStruct, TestResource> method("testMethod");

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

TEST(TSdlOwnerMethodTest, SupplyParameters)
{
	{
		TSdlOwnerMethod<TestMethodStruct, TestResource> method("ttt");
		method.addParam(
			TSdlString<TestMethodStruct>("someValue", &TestMethodStruct::str));
		EXPECT_EQ(method.numParams(), 1);

		ASSERT_TRUE(method.getParam(0) != nullptr);
		PH_EXPECT_STRING_EQ(method.getParam(0)->getTypeName(), "string");
		PH_EXPECT_STRING_EQ(method.getParam(0)->getFieldName(), "someValue");

		// Getting out-of-bound parameters is allowed
		for(std::size_t i = 1; i < 1000; ++i)
		{
			EXPECT_TRUE(method.getParam(i) == nullptr);
		}
	}
}

TEST(TSdlOwnerMethodTest, CallMethod)
{
	{
		TSdlOwnerMethod<TestMethodStruct, TestResource> method("setToHello");
		method.addParam(
			TSdlString<TestMethodStruct>("someParam", &TestMethodStruct::str));

		TestResource res;
		res.str = "today is a nice day";

		SdlInputClauses clauses;
		clauses.add("string", "someParam", "hello");

		// The method should set the <str> field in the resource to "hello"
		method.call(&res, clauses, SdlInputContext());

		PH_EXPECT_STRING_EQ(res.str, "hello");
	}
}
