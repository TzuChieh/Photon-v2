#include "util.h"

#include <DataIO/SDL/Introspect/TSdlNestedField.h>
#include <DataIO/SDL/Introspect/TSdlString.h>

#include <gtest/gtest.h>

#include <string>
#include <type_traits>

using namespace ph;

namespace
{

struct TestInner
{
	std::string str;
};

struct TestOuter
{
	TestInner innerObj;
};

}// end namespace

// SDL string is the most trivial field as it simply copies the input

TEST(TSdlNestedFieldTest, RequiredProperties)
{
	{
		using FieldType = TSdlNestedField<TestOuter, TestInner>;
		EXPECT_TRUE(std::is_copy_constructible_v<FieldType>);
		EXPECT_TRUE(std::is_move_constructible_v<FieldType>);
	}
}

TEST(TSdlNestedFieldTest, KeepsFieldSettings)
{
	{
		TSdlString<TestInner> innerStrField("test-str", &TestInner::str);
		innerStrField.description("test description");
		innerStrField.enableFallback(false);
		innerStrField.withImportance(EFieldImportance::Required);

		TSdlNestedField<TestOuter, TestInner> nestedField(&TestOuter::innerObj, &innerStrField);

		PH_EXPECT_STRING_EQ(nestedField.getTypeName(), innerStrField.getTypeName());
		PH_EXPECT_STRING_EQ(nestedField.getFieldName(), innerStrField.getFieldName());
		PH_EXPECT_STRING_EQ(nestedField.getDescription(), innerStrField.getDescription());
		EXPECT_EQ(nestedField.getImportance(), innerStrField.getImportance());
		EXPECT_EQ(nestedField.isFallbackEnabled(), innerStrField.isFallbackEnabled());
	}
}

// TODO: read from sdl
