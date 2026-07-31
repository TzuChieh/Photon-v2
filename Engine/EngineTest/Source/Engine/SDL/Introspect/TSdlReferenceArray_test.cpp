#include "engine_test_util.h"
#include "Engine/SDL/Introspect/TSdlReferenceArray_test.h"

#include <Engine/SDL/Introspect/TSdlReference.h>
#include <Engine/SDL/Introspect/TSdlReferenceArray.h>
#include <Engine/SDL/Introspect/SdlInputContext.h>
#include <Engine/SDL/Introspect/SdlOutputContext.h>
#include <Engine/SDL/RawResourceCollection.h>
#include <Engine/SDL/SdlInputClause.h>
#include <Engine/SDL/SdlOutputClause.h>
#include <Engine/DataIO/FileSystem/Path.h>

#include <gtest/gtest.h>

#include <type_traits>
#include <utility>

using namespace ph;
using namespace ph::TSdlReferenceArray_test;

TEST(TSdlReferenceArrayTest, RequiredProperties)
{
	using FieldType = TSdlReferenceArray<TestResource, TestReferenceArrayOwner>;
	EXPECT_TRUE(std::is_copy_constructible_v<FieldType>);
	EXPECT_TRUE(std::is_move_constructible_v<FieldType>);
}

TEST(TSdlReferenceTest, ReadEmptyReferenceFromSdl)
{
	SdlInputClause clause;
	clause.valueType = ESdlClauseValue::General;
	clause.value = "";

	using FieldType = TSdlReference<TestResource, TestReferenceArrayOwner>;
	EXPECT_EQ(FieldType::loadReference(clause, SdlInputContext()), nullptr);
}

TEST(TSdlReferenceArrayTest, ReadSingleReferenceFromSdl)
{
	TSdlReferenceArray<TestResource, TestReferenceArrayOwner> sdlRefArr("values", &TestReferenceArrayOwner::values);

	// Create one named resource as the reference target.
	RawResourceCollection resources;
	auto resource = std::make_shared<TestResource>();
	resources.add(resource, "single");

	Path workingDirectory(".");
	SdlInputContext ctx(&resources, nullptr, &workingDirectory);

	// Build clause directly to isolate array loading from inline packet parsing.
	SdlInputClause clause;
	clause.type = "object-array";
	clause.name = "values";
	clause.valueType = ESdlClauseValue::PersistentTargetName;
	clause.value = "single";

	TestReferenceArrayOwner owner;
	EXPECT_NO_THROW(sdlRefArr.fromSdl(owner, clause, ctx));
	ASSERT_EQ(owner.values.size(), 1);
	EXPECT_EQ(owner.values[0], resource);
}

TEST(TSdlReferenceArrayTest, ReadReferenceArrayFromSdl)
{
	TSdlReferenceArray<TestResource, TestReferenceArrayOwner> sdlRefArr("values", &TestReferenceArrayOwner::values);

	RawResourceCollection resources;
	auto left = std::make_shared<TestResource>();
	auto right = std::make_shared<TestResource>();
	resources.add(left, "left");
	resources.add(right, "right");
	
	Path workingDirectory(".");
	SdlInputContext ctx(&resources, nullptr, &workingDirectory);

	// Keep this test focused on list-to-array semantics.
	SdlInputClause clause;
	clause.type = "object-array";
	clause.name = "values";
	clause.valueType = ESdlClauseValue::General;
	clause.value = "@left @right";

	TestReferenceArrayOwner owner;
	EXPECT_NO_THROW(sdlRefArr.fromSdl(owner, clause, ctx));
	ASSERT_EQ(owner.values.size(), 2);
	EXPECT_EQ(owner.values[0], left);
	EXPECT_EQ(owner.values[1], right);
}

TEST(TSdlReferenceArrayTest, ReadReferenceArrayFromSdlWithEmptySlot)
{
	TSdlReferenceArray<TestResource, TestReferenceArrayOwner> sdlRefArr("values", &TestReferenceArrayOwner::values);

	RawResourceCollection resources;
	auto left = std::make_shared<TestResource>();
	auto right = std::make_shared<TestResource>();
	resources.add(left, "left");
	resources.add(right, "right");

	Path workingDirectory(".");
	SdlInputContext ctx(&resources, nullptr, &workingDirectory);

	SdlInputClause clause;
	clause.type = "object-array";
	clause.name = "values";
	clause.valueType = ESdlClauseValue::General;
	clause.value = "@left \"\" @right";

	TestReferenceArrayOwner owner;
	EXPECT_NO_THROW(sdlRefArr.fromSdl(owner, clause, ctx));
	ASSERT_EQ(owner.values.size(), 3);
	EXPECT_EQ(owner.values[0], left);
	EXPECT_EQ(owner.values[1], nullptr);
	EXPECT_EQ(owner.values[2], right);

	std::vector<const ISdlResource*> resourcesOwnedByField;
	sdlRefArr.ownedResources(owner, resourcesOwnedByField);
	ASSERT_EQ(resourcesOwnedByField.size(), 2);
	EXPECT_EQ(resourcesOwnedByField[0], left.get());
	EXPECT_EQ(resourcesOwnedByField[1], right.get());
}

TEST(TSdlReferenceArrayTest, WriteEmptyReferenceSlotToSdl)
{
	TSdlReferenceArray<TestResource, TestReferenceArrayOwner> sdlRefArr("values", &TestReferenceArrayOwner::values);

	TestReferenceArrayOwner owner;
	owner.values = {nullptr};

	Path workingDirectory(".");
	SdlOutputContext ctx(&workingDirectory);
	SdlOutputClause clause;
	EXPECT_NO_THROW(sdlRefArr.toSdl(owner, clause, ctx));
	EXPECT_FALSE(clause.isEmpty);
	EXPECT_EQ(clause.value, "{\"\" }");
}

TEST(TSdlReferenceArrayTest, ReadReferenceArrayFromSdlWithSplitSpecifier)
{
	TSdlReferenceArray<TestResource, TestReferenceArrayOwner> sdlRefArr("values", &TestReferenceArrayOwner::values);

	// Also accept split specifier-name form for compatibility
	RawResourceCollection resources;
	auto left = std::make_shared<TestResource>();
	auto right = std::make_shared<TestResource>();
	resources.add(left, "left");
	resources.add(right, "right");

	Path workingDirectory(".");
	SdlInputContext ctx(&resources, nullptr, &workingDirectory);

	SdlInputClause clause;
	clause.type = "object-array";
	clause.name = "values";
	clause.valueType = ESdlClauseValue::General;
	clause.value = "@ left @ right";

	TestReferenceArrayOwner owner;
	EXPECT_NO_THROW(sdlRefArr.fromSdl(owner, clause, ctx));
	ASSERT_EQ(owner.values.size(), 2);
	EXPECT_EQ(owner.values[0], left);
	EXPECT_EQ(owner.values[1], right);
}

TEST(TSdlReferenceArrayTest, ReadEmptyReferenceArrayFromSdl)
{
	TSdlReferenceArray<TestResource, TestReferenceArrayOwner> sdlRefArr("values", &TestReferenceArrayOwner::values);

	RawResourceCollection resources;
	Path workingDirectory(".");
	SdlInputContext ctx(&resources, nullptr, &workingDirectory);

	SdlInputClause clause;
	clause.type = "object-array";
	clause.name = "values";
	clause.valueType = ESdlClauseValue::General;
	clause.value = "";

	TestReferenceArrayOwner owner;
	EXPECT_NO_THROW(sdlRefArr.fromSdl(owner, clause, ctx));
	EXPECT_TRUE(owner.values.empty());
}

TEST(TSdlReferenceArrayTest, ReadReferenceArrayFromSdlFallsBackOnTrailingNonSpecifierNameToken)
{
	TSdlReferenceArray<TestResource, TestReferenceArrayOwner> sdlRefArr("values", &TestReferenceArrayOwner::values);

	RawResourceCollection resources;
	auto left = std::make_shared<TestResource>();
	resources.add(left, "left");

	Path workingDirectory(".");
	SdlInputContext ctx(&resources, nullptr, &workingDirectory);

	SdlInputClause clause;
	clause.type = "object-array";
	clause.name = "values";
	clause.valueType = ESdlClauseValue::General;
	clause.value = "@left trailing";

	TestReferenceArrayOwner owner;
	owner.values = {left};

	// Should fallback to empty array due to malformed input
	EXPECT_NO_THROW(sdlRefArr.fromSdl(owner, clause, ctx));
	EXPECT_TRUE(owner.values.empty());
}
