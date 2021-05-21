#include <DataIO/SDL/Introspect/TBasicSdlFieldSet.h>
#include <DataIO/SDL/Introspect/TSdlString.h>
#include <DataIO/SDL/Introspect/TSdlInteger.h>
#include <DataIO/SDL/Introspect/TSdlReal.h>

#include <gtest/gtest.h>

#include <string>
#include <utility>

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

TEST(TBasicSdlFieldSetTest, AddFields)
{
	// SDL string is the most trivial field as it simply copies the input

	{
		TBasicSdlFieldSet<TSdlString<TestOwner>> fieldSetA;
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
		}

		TBasicSdlFieldSet<TSdlString<TestOwner>> fieldSetB;
		{
			EXPECT_EQ(fieldSetB.numFields(), 0);

			fieldSetB.addField(TSdlString<TestOwner>("str-in-b", &TestOwner::s));
			EXPECT_EQ(fieldSetB.numFields(), 1);
			EXPECT_STREQ(fieldSetB[0].getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetB[0].getFieldName().c_str(), "str-in-b");

			fieldSetB.addFields(std::move(fieldSetA));
			EXPECT_EQ(fieldSetB.numFields(), 4);
			EXPECT_STREQ(fieldSetB[1].getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetB[1].getFieldName().c_str(), "nameA");
			EXPECT_STREQ(fieldSetB[2].getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetB[2].getFieldName().c_str(), "nameB");
			EXPECT_STREQ(fieldSetB[3].getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetB[3].getFieldName().c_str(), "nameC");

			fieldSetB.addField(TSdlString<TestOwner>("str-in-b-2", &TestOwner::s));
			EXPECT_EQ(fieldSetB.numFields(), 5);
			EXPECT_STREQ(fieldSetB[4].getTypeName().c_str(), "string");
			EXPECT_STREQ(fieldSetB[4].getFieldName().c_str(), "str-in-b-2");
		}
		// Note: <fieldSetA> has been moved from
	}
}

TEST(TBasicSdlFieldSetTest, AddPolymorphicFields)
{
	{
		TBasicSdlFieldSet<TOwnedSdlField<TestOwner>> fieldSetA;
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

		TBasicSdlFieldSet<TOwnedSdlField<TestOwner>> fieldSetB;
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
