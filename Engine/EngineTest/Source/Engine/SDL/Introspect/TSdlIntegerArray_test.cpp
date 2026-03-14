#include "engine_test_util.h"

#include <Engine/SDL/Introspect/TSdlIntegerArray.h>

#include <gtest/gtest.h>

#include <vector>
#include <type_traits>

using namespace ph;

namespace
{
	class IntArrOwner
	{
	public:
		std::vector<integer> arr;
	};
}

TEST(TSdlIntegerArrayTest, RequiredProperties)
{
	{
		using FieldType = TSdlIntegerArray<IntArrOwner>;
		EXPECT_TRUE(std::is_copy_constructible_v<FieldType>);
		EXPECT_TRUE(std::is_move_constructible_v<FieldType>);
	}
}

TEST(TSdlIntegerArrayTest, ReadFromSdl)
{
	{
		TSdlIntegerArray<IntArrOwner> sdlIntArr("array", &IntArrOwner::arr);

		// Default initialization performed on default value by default
		ASSERT_TRUE(sdlIntArr.getDefaultValue());
		EXPECT_TRUE(sdlIntArr.getDefaultValue()->empty());

		IntArrOwner owner;
		SdlInputContext ctx;

		EXPECT_NO_THROW(sdlIntArr.fromSdl(owner, SdlInputClause("1 2 3 -456"), ctx));
		ASSERT_TRUE(owner.arr.size() == 4);
		EXPECT_EQ(owner.arr[0], 1);
		EXPECT_EQ(owner.arr[1], 2);
		EXPECT_EQ(owner.arr[2], 3);
		EXPECT_EQ(owner.arr[3], -456);

		// Fallback to default for optional & nice-to-have fields

		sdlIntArr.defaultTo({0});
		sdlIntArr.optional();
		EXPECT_NO_THROW(sdlIntArr.fromSdl(owner, SdlInputClause("Wowowowow"), ctx));
		ASSERT_TRUE(owner.arr.size() == 1);
		EXPECT_EQ(owner.arr[0], 0);

		sdlIntArr.defaultTo({-2, -1});
		sdlIntArr.niceToHave();
		EXPECT_NO_THROW(sdlIntArr.fromSdl(owner, SdlInputClause("testing"), ctx));
		ASSERT_TRUE(owner.arr.size() == 2);
		EXPECT_EQ(owner.arr[0], -2);
		EXPECT_EQ(owner.arr[1], -1);
	}

	// Disable fallback for fields
	{
		TSdlIntegerArray<IntArrOwner> sdlIntArr("array3", &IntArrOwner::arr);
		sdlIntArr.options({EFieldOption::DisableFallback});
		sdlIntArr.defaultTo({123, 456, 789});

		IntArrOwner owner;
		owner.arr = {6, -8};

		SdlInputContext ctx;

		EXPECT_THROW(sdlIntArr.fromSdl(owner, SdlInputClause("intArrYoyo"), ctx), SdlLoadError);
		ASSERT_TRUE(owner.arr.size() == 2);
		EXPECT_EQ(owner.arr[0],  6);// owner value should not update
		EXPECT_EQ(owner.arr[1], -8);//
	}
}

TEST(TSdlIntegerArrayTest, NativeData)
{
	TSdlIntegerArray<IntArrOwner> sdlIntArr("array", &IntArrOwner::arr);
	IntArrOwner owner;
	owner.arr = {-1, 2, 3};

	SdlNativeData data = sdlIntArr.ownedNativeData(owner);
	EXPECT_EQ(data.numElements, 3);
	EXPECT_EQ(data.elementContainer, ESdlDataFormat::Vector);
	EXPECT_EQ(data.elementType, sdl::int_type_of<integer>());

	ASSERT_TRUE(data);

	// Access by native data getter
	EXPECT_EQ(data.get<int64>(0), -1);
	EXPECT_EQ(data.get<int64>(1), 2);
	EXPECT_EQ(data.get<int64>(2), 3);

	// Which is what native data actually stored
	EXPECT_EQ(owner.arr[0], -1);
	EXPECT_EQ(owner.arr[1], 2);
	EXPECT_EQ(owner.arr[2], 3);

	// Modify `arr[1]` by native data setter
	EXPECT_TRUE(data.set<int64>(1, 22));

	// Access by native data getter again after update
	EXPECT_EQ(data.get<int64>(0), -1);
	EXPECT_EQ(data.get<int64>(1), 22);// <- changed by setter
	EXPECT_EQ(data.get<int64>(2), 3);

	// Which is what native data actually stored
	EXPECT_EQ(owner.arr[0], -1);
	EXPECT_EQ(owner.arr[1], 22);// <- changed by setter
	EXPECT_EQ(owner.arr[2], 3);
}
