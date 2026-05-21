#include "engine_test_util.h"
#include "Engine/SDL/Introspect/TSdlStructArray_test.h"

#include <Engine/SDL/Introspect/TSdlStructArray.h>
#include <Engine/SDL/Introspect/SdlInputContext.h>
#include <Engine/SDL/SdlDataPacketCollection.h>
#include <Engine/SDL/SdlInputClause.h>
#include <Engine/SDL/SdlInputClauses.h>
#include <Engine/DataIO/FileSystem/Path.h>

#include <gtest/gtest.h>

#include <type_traits>
#include <utility>

using namespace ph;
using namespace ph::TSdlStructArray_test;

TEST(TSdlStructArrayTest, RequiredProperties)
{
	using FieldType = TSdlStructArray<TestStruct, TestStructArrayOwner>;
	EXPECT_TRUE(std::is_copy_constructible_v<FieldType>);
	EXPECT_TRUE(std::is_move_constructible_v<FieldType>);
}

TEST(TSdlStructArrayTest, ReadSingleStructFromSdl)
{
	TSdlStructArray<TestStruct, TestStructArrayOwner> sdlStructArr("values", &TestStructArrayOwner::values);

	// Create one named packet as the struct payload
	SdlDataPacketCollection packets;
	{
		SdlInputClauses packet;
		packet.add("integer", "value", "12");
		packets.addUnique(std::move(packet), "single");
	}

	Path workingDirectory(".");
	SdlInputContext ctx(nullptr, &packets, &workingDirectory);

	// Build clause directly to isolate array loading from inline packet parsing.
	SdlInputClause clause;
	clause.type = "struct-array";
	clause.name = "values";
	clause.valueType = ESdlClauseValue::CachedTargetName;
	clause.value = "single";

	// Load from SDL and verify one struct element is materialized
	TestStructArrayOwner owner;
	EXPECT_NO_THROW(sdlStructArr.fromSdl(owner, clause, ctx));
	ASSERT_EQ(owner.values.size(), 1);
	EXPECT_EQ(owner.values[0].value, 12);
}

TEST(TSdlStructArrayTest, ReadStructArrayFromSdl)
{
	TSdlStructArray<TestStruct, TestStructArrayOwner> sdlStructArr("values", &TestStructArrayOwner::values);

	// Create two named packets to represent two struct entries
	SdlDataPacketCollection packets;
	{
		SdlInputClauses packet;
		packet.add("integer", "value", "3");
		packets.addUnique(std::move(packet), "first");
	}
	{
		SdlInputClauses packet;
		packet.add("integer", "value", "7");
		packets.addUnique(std::move(packet), "second");
	}

	Path workingDirectory(".");
	SdlInputContext ctx(nullptr, &packets, &workingDirectory);

	// Keep this test focused on list-to-array semantics.
	SdlInputClause clause;
	clause.type = "struct-array";
	clause.name = "values";
	clause.valueType = ESdlClauseValue::General;
	clause.value = "$first $second";

	// Multi-entry struct-array should preserve packet order
	TestStructArrayOwner owner;
	EXPECT_NO_THROW(sdlStructArr.fromSdl(owner, clause, ctx));
	ASSERT_EQ(owner.values.size(), 2);
	EXPECT_EQ(owner.values[0].value, 3);
	EXPECT_EQ(owner.values[1].value, 7);
}

TEST(TSdlStructArrayTest, ReadStructArrayFromSdlWithSplitSpecifier)
{
	TSdlStructArray<TestStruct, TestStructArrayOwner> sdlStructArr("values", &TestStructArrayOwner::values);

	// Also accept split specifier-name form for compatibility
	SdlDataPacketCollection packets;
	{
		SdlInputClauses packet;
		packet.add("integer", "value", "2");
		packets.addUnique(std::move(packet), "left");
	}
	{
		SdlInputClauses packet;
		packet.add("integer", "value", "9");
		packets.addUnique(std::move(packet), "right");
	}

	Path workingDirectory(".");
	SdlInputContext ctx(nullptr, &packets, &workingDirectory);

	SdlInputClause clause;
	clause.type = "struct-array";
	clause.name = "values";
	clause.valueType = ESdlClauseValue::General;
	clause.value = "$ left $ right";

	TestStructArrayOwner owner;
	EXPECT_NO_THROW(sdlStructArr.fromSdl(owner, clause, ctx));
	ASSERT_EQ(owner.values.size(), 2);
	EXPECT_EQ(owner.values[0].value, 2);
	EXPECT_EQ(owner.values[1].value, 9);
}

TEST(TSdlStructArrayTest, ReadEmptyStructArrayFromSdl)
{
	TSdlStructArray<TestStruct, TestStructArrayOwner> sdlStructArr("values", &TestStructArrayOwner::values);

	SdlDataPacketCollection packets;
	Path workingDirectory(".");
	SdlInputContext ctx(nullptr, &packets, &workingDirectory);

	SdlInputClause clause;
	clause.type = "struct-array";
	clause.name = "values";
	clause.valueType = ESdlClauseValue::General;
	clause.value = "";

	TestStructArrayOwner owner;
	EXPECT_NO_THROW(sdlStructArr.fromSdl(owner, clause, ctx));
	EXPECT_TRUE(owner.values.empty());
}

TEST(TSdlStructArrayTest, ReadStructArrayFromSdlFallsBackOnTrailingNonCacheNameToken)
{
	TSdlStructArray<TestStruct, TestStructArrayOwner> sdlStructArr("values", &TestStructArrayOwner::values);

	SdlDataPacketCollection packets;
	{
		SdlInputClauses packet;
		packet.add("integer", "value", "5");
		packets.addUnique(std::move(packet), "first");
	}

	Path workingDirectory(".");
	SdlInputContext ctx(nullptr, &packets, &workingDirectory);

	SdlInputClause clause;
	clause.type = "struct-array";
	clause.name = "values";
	clause.valueType = ESdlClauseValue::General;
	clause.value = "$first trailing";

	TestStructArrayOwner owner;
	owner.values = {{.value = 42}};

	// Should fallback to empty array due to malformed input
	EXPECT_NO_THROW(sdlStructArr.fromSdl(owner, clause, ctx));
	EXPECT_TRUE(owner.values.empty());
}
