#include "util.h"

#include <DataIO/SDL/Introspect/TSdlMethod.h>
#include <DataIO/SDL/ISdlResource.h>
#include <DataIO/SDL/Introspect/TSdlString.h>
#include <DataIO/SDL/ValueClauses.h>

#include <gtest/gtest.h>

#include <string>
#include <cstddef>

using namespace ph;

namespace
{

class TestResource : public ISdlResource
{
public:
	std::string str;

	ETypeCategory getCategory() const override
	{
		return ETypeCategory::REF_OPTION;
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

TEST(TSdlMethodTest, SupplyParameters)
{
	{
		TSdlMethod<TestMethodStruct, TestResource> method("ttt");
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

TEST(TSdlMethodTest, CallMethod)
{
	{
		TSdlMethod<TestMethodStruct, TestResource> method("setToHello");
		method.addParam(
			TSdlString<TestMethodStruct>("someParam", &TestMethodStruct::str));

		TestResource res;
		res.str = "today is a nice day";

		ValueClauses clauses;
		clauses.add("string", "someParam", "hello");

		// The method should set the <str> field in the resource to "hello"
		method.call(&res, clauses, SdlInputContext());

		PH_EXPECT_STRING_EQ(res.str, "hello");
	}
}
