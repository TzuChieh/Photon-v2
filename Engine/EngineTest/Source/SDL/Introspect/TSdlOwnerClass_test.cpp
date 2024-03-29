#include "util.h"

#include <SDL/Introspect/TSdlOwnerClass.h>
#include <SDL/ISdlResource.h>
#include <Common/primitive_type.h>
#include <SDL/Introspect/TSdlReal.h>
#include <SDL/Introspect/TSdlString.h>
#include <SDL/Introspect/TSdlMethod.h>
#include <SDL/sdl_interface.h>

#include <gtest/gtest.h>

#include <string>
#include <cstddef>
#include <type_traits>

using namespace ph;

namespace
{

class TestResource : public ISdlResource
{
public:
	static constexpr ESdlTypeCategory CATEGORY = ESdlTypeCategory::Ref_Geometry;

	real r;
	std::string str;

	ESdlTypeCategory getDynamicCategory() const override
	{
		return CATEGORY;
	}

	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<TestResource>)
	{
		return ClassType("testClass");
	}
};

struct TestMethodStruct
{
	void operator () (TestResource& res)
	{}

	PH_DEFINE_SDL_FUNCTION(TSdlMethod<TestMethodStruct, TestResource>)
	{
		return FunctionType("testMethod");
	}
};

struct TestMethodStruct2
{
	void operator () (const TestResource& res) const
	{}

	PH_DEFINE_SDL_FUNCTION(TSdlMethod<TestMethodStruct2, TestResource>)
	{
		return FunctionType("testMethod2");
	}
};

}// end anonymous namespace

TEST(TSdlOwnerClassTest, RequiredProperties)
{
	{
		using SdlClassType = TSdlOwnerClass<TestResource>;

		// Non-copyable as it is not needed--just access via reference.
		EXPECT_FALSE(std::is_copy_constructible_v<SdlClassType>);
		EXPECT_FALSE(std::is_copy_assignable_v<SdlClassType>);

		// Move is allowed for SDL class construction phase.
		EXPECT_TRUE(std::is_move_constructible_v<SdlClassType>);
		EXPECT_TRUE(std::is_move_assignable_v<SdlClassType>);
	}
}

TEST(TSdlOwnerClassTest, DefaultStates)
{
	{
		TSdlOwnerClass<TestResource> sdlClass("testName");

		EXPECT_EQ(sdlClass.numFields(), 0);
		EXPECT_EQ(sdlClass.numFunctions(), 0);
		EXPECT_STREQ(sdlClass.genCategoryName().c_str(), "geometry");
		EXPECT_EQ(sdlClass.getTypeName(), "testName");
		EXPECT_EQ(sdlClass.getDescription(), "");

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

TEST(TSdlOwnerClassTest, AddAndGetFields)
{
	{
		TSdlOwnerClass<TestResource> sdlClass("testName");
		sdlClass.addField(TSdlReal<TestResource>("testReal", &TestResource::r));
		EXPECT_EQ(sdlClass.numFields(), 1);

		const auto field0 = sdlClass.getField(0);
		ASSERT_TRUE(field0 != nullptr);

		PH_EXPECT_STRING_EQ(field0->getTypeName(), "real");
		PH_EXPECT_STRING_EQ(field0->getFieldName(), "testReal");

		// Add one more field

		sdlClass.addField(TSdlString<TestResource>("testString", &TestResource::str));
		EXPECT_EQ(sdlClass.numFields(), 2);

		const auto field1 = sdlClass.getField(1);
		ASSERT_TRUE(field1 != nullptr);

		PH_EXPECT_STRING_EQ(field1->getTypeName(), "string");
		PH_EXPECT_STRING_EQ(field1->getFieldName(), "testString");

		// Getting out-of-bound fields is allowed
		for(std::size_t i = 2; i < 1000; ++i)
		{
			EXPECT_TRUE(sdlClass.getField(i) == nullptr);
			EXPECT_TRUE(sdlClass.getOwnedField(i) == nullptr);
		}
	}
}

TEST(TSdlOwnerClassTest, AddAndGetFunctions)
{
	{
		TSdlOwnerClass<TestResource> sdlClass("testName");
		sdlClass.addFunction<TestMethodStruct>();
		EXPECT_EQ(sdlClass.numFunctions(), 1);

		const auto func = sdlClass.getFunction(0);
		ASSERT_TRUE(func != nullptr);
		PH_EXPECT_STRING_EQ(func->getName(), "testMethod");

		// Add one more function

		sdlClass.addFunction<TestMethodStruct2>();
		EXPECT_EQ(sdlClass.numFunctions(), 2);

		const auto func1 = sdlClass.getFunction(1);
		ASSERT_TRUE(func1 != nullptr);
		PH_EXPECT_STRING_EQ(func1->getName(), "testMethod2");

		// Getting out-of-bound functions is allowed
		for(std::size_t i = 2; i < 1000; ++i)
		{
			EXPECT_TRUE(sdlClass.getFunction(i) == nullptr);
		}
	}
}
