#include <SDL/Introspect/TSdlBruteForceFieldSet.h>
#include <SDL/Introspect/TSdlString.h>
#include <SDL/Introspect/TSdlInteger.h>
#include <SDL/Introspect/TSdlReal.h>

#include <gtest/gtest.h>

#include <string>
#include <utility>
#include <type_traits>

using namespace ph;

namespace
{
	struct TestOwner
	{
		std::string s;
		integer i;
		real r;
	};
}

TEST(TSdlBruteForceFieldSetTest, RequiredProperties)
{
	// SDL string is the most trivial field as it simply copies the input

	{
		using FieldSet = TSdlBruteForceFieldSet<TSdlString<TestOwner>>;
		EXPECT_FALSE(std::is_copy_constructible_v<FieldSet>);
		EXPECT_TRUE(std::is_move_constructible_v<FieldSet>);
		EXPECT_FALSE(std::is_copy_assignable_v<FieldSet>);
		EXPECT_TRUE(std::is_move_assignable_v<FieldSet>);
	}
}

TEST(TSdlBruteForceFieldSetTest, AddFields)
{
	// SDL string is the most trivial field as it simply copies the input

	{
		TSdlBruteForceFieldSet<TSdlString<TestOwner>> fieldSetA;
		{
			EXPECT_EQ(fieldSetA.numFields(), 0);

			fieldSetA.addField(TSdlString<TestOwner>("nameA", &TestOwner::s));
			fieldSetA.addField(TSdlString<TestOwner>("nameB", &TestOwner::s));
			fieldSetA.addField(TSdlString<TestOwner>("nameC", &TestOwner::s));
			EXPECT_EQ(fieldSetA.numFields(), 3);
			EXPECT_STREQ(fieldSetA[0].getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetA[0].getFieldName().c_str(), "nameA");
			EXPECT_STREQ(fieldSetA[1].getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetA[1].getFieldName().c_str(), "nameB");
			EXPECT_STREQ(fieldSetA[2].getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetA[2].getFieldName().c_str(), "nameC");

			ASSERT_TRUE(fieldSetA.getField(0) != nullptr);
			EXPECT_STREQ(fieldSetA.getField(0)->getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetA.getField(0)->getFieldName().c_str(), "nameA");
			ASSERT_TRUE(fieldSetA.getField(1) != nullptr);
			EXPECT_STREQ(fieldSetA.getField(1)->getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetA.getField(1)->getFieldName().c_str(), "nameB");
			ASSERT_TRUE(fieldSetA.getField(2) != nullptr);
			EXPECT_STREQ(fieldSetA.getField(2)->getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetA.getField(2)->getFieldName().c_str(), "nameC");

			// Getting non-existent fields is allowed and the result will be nullptr
			for(std::size_t fieldIdx = 3; fieldIdx < 1024; ++fieldIdx)
			{
				EXPECT_TRUE(fieldSetA.getField(fieldIdx) == nullptr);
			}
		}

		TSdlBruteForceFieldSet<TSdlString<TestOwner>> fieldSetB;
		{
			EXPECT_EQ(fieldSetB.numFields(), 0);

			fieldSetB.addField(TSdlString<TestOwner>("str-in-b", &TestOwner::s));
			EXPECT_EQ(fieldSetB.numFields(), 1);
			EXPECT_STREQ(fieldSetB[0].getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetB[0].getFieldName().c_str(), "str-in-b");

			ASSERT_FALSE(fieldSetB.getField(0) == nullptr);
			EXPECT_STREQ(fieldSetB.getField(0)->getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetB.getField(0)->getFieldName().c_str(), "str-in-b");

			fieldSetB.addFields(std::move(fieldSetA));
			EXPECT_EQ(fieldSetB.numFields(), 4);
			EXPECT_STREQ(fieldSetB[1].getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetB[1].getFieldName().c_str(), "nameA");
			EXPECT_STREQ(fieldSetB[2].getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetB[2].getFieldName().c_str(), "nameB");
			EXPECT_STREQ(fieldSetB[3].getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetB[3].getFieldName().c_str(), "nameC");

			ASSERT_TRUE(fieldSetB.getField(1) != nullptr);
			EXPECT_STREQ(fieldSetB.getField(1)->getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetB.getField(1)->getFieldName().c_str(), "nameA");
			ASSERT_TRUE(fieldSetB.getField(2) != nullptr);
			EXPECT_STREQ(fieldSetB.getField(2)->getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetB.getField(2)->getFieldName().c_str(), "nameB");
			ASSERT_TRUE(fieldSetB.getField(3) != nullptr);
			EXPECT_STREQ(fieldSetB.getField(3)->getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetB.getField(3)->getFieldName().c_str(), "nameC");

			fieldSetB.addField(TSdlString<TestOwner>("str-in-b-2", &TestOwner::s));
			EXPECT_EQ(fieldSetB.numFields(), 5);
			EXPECT_STREQ(fieldSetB[4].getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetB[4].getFieldName().c_str(), "str-in-b-2");

			ASSERT_TRUE(fieldSetB.getField(4) != nullptr);
			EXPECT_STREQ(fieldSetB.getField(4)->getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetB.getField(4)->getFieldName().c_str(), "str-in-b-2");

			// Getting non-existent fields is allowed and the result will be nullptr
			for(std::size_t fieldIdx = 5; fieldIdx < 1024; ++fieldIdx)
			{
				EXPECT_TRUE(fieldSetB.getField(fieldIdx) == nullptr);
			}
		}
		// Note: <fieldSetA> has been moved from
	}
}

TEST(TSdlBruteForceFieldSetTest, AddPolymorphicFields)
{
	{
		TSdlBruteForceFieldSet<TSdlOwnedField<TestOwner>> fieldSetA;
		{
			EXPECT_EQ(fieldSetA.numFields(), 0);

			fieldSetA.addField(TSdlString<TestOwner>("strA", &TestOwner::s));
			fieldSetA.addField(TSdlInteger<TestOwner>("intA", &TestOwner::i));
			fieldSetA.addField(TSdlReal<TestOwner>("realA", &TestOwner::r));
			EXPECT_EQ(fieldSetA.numFields(), 3);
			EXPECT_STREQ(fieldSetA[0].getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetA[0].getFieldName().c_str(), "strA");
			EXPECT_STREQ(fieldSetA[1].getTypeName().c_str(), "integer");
			EXPECT_STREQ(fieldSetA[1].getFieldName().c_str(), "intA");
			EXPECT_STREQ(fieldSetA[2].getTypeName().c_str(), "real");
			EXPECT_STREQ(fieldSetA[2].getFieldName().c_str(), "realA");
		}

		TSdlBruteForceFieldSet<TSdlOwnedField<TestOwner>> fieldSetB;
		{
			EXPECT_EQ(fieldSetB.numFields(), 0);

			fieldSetB.addField(TSdlInteger<TestOwner>("int-in-b", &TestOwner::i));
			EXPECT_EQ(fieldSetB.numFields(), 1);
			EXPECT_STREQ(fieldSetB[0].getTypeName().c_str(), "integer");
			EXPECT_STREQ(fieldSetB[0].getFieldName().c_str(), "int-in-b");

			fieldSetB.addFields(std::move(fieldSetA));
			EXPECT_EQ(fieldSetB.numFields(), 4);
			EXPECT_STREQ(fieldSetB[1].getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetB[1].getFieldName().c_str(), "strA");
			EXPECT_STREQ(fieldSetB[2].getTypeName().c_str(), "integer");
			EXPECT_STREQ(fieldSetB[2].getFieldName().c_str(), "intA");
			EXPECT_STREQ(fieldSetB[3].getTypeName().c_str(), "real");
			EXPECT_STREQ(fieldSetB[3].getFieldName().c_str(), "realA");

			fieldSetB.addField(TSdlReal<TestOwner>("real-in-b", &TestOwner::r));
			EXPECT_EQ(fieldSetB.numFields(), 5);
			EXPECT_STREQ(fieldSetB[4].getTypeName().c_str(), "real");
			EXPECT_STREQ(fieldSetB[4].getFieldName().c_str(), "real-in-b");
		}
		// Note: <fieldSetA> has been moved from
	}
}
