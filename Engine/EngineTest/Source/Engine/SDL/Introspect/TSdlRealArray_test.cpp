#include "engine_test_util.h"

#include <Engine/SDL/Introspect/TSdlRealArray.h>

#include <gtest/gtest.h>

#include <vector>
#include <type_traits>

using namespace ph;

namespace
{
	class RealArrOwner
	{
	public:
		std::vector<real> arr;
	};
}

TEST(TSdlRealArrayTest, RequiredProperties)
{
	{
		using FieldType = TSdlRealArray<RealArrOwner>;
		EXPECT_TRUE(std::is_copy_constructible_v<FieldType>);
		EXPECT_TRUE(std::is_move_constructible_v<FieldType>);
	}
}

TEST(TSdlRealArrayTest, ReadFromSdl)
{
	{
		TSdlRealArray<RealArrOwner> sdlRealArr("array", &RealArrOwner::arr);

		// Default initialization performed on default value by default
		ASSERT_TRUE(sdlRealArr.getDefaultValue());
		EXPECT_TRUE(sdlRealArr.getDefaultValue()->empty());

		RealArrOwner owner;
		SdlInputContext ctx;

		EXPECT_NO_THROW(sdlRealArr.fromSdl(owner, SdlInputClause("1.2    2   3.876 -456.789"), ctx));
		ASSERT_TRUE(owner.arr.size() == 4);
		PH_EXPECT_REAL_EQ(owner.arr[0], 1.2_r);
		PH_EXPECT_REAL_EQ(owner.arr[1], 2.0_r);
		PH_EXPECT_REAL_EQ(owner.arr[2], 3.876_r);
		PH_EXPECT_REAL_EQ(owner.arr[3], -456.789_r);

		// TODO: scientific notation

		// Fallback to default for optional & nice-to-have fields

		sdlRealArr.defaultTo({0.0_r});
		sdlRealArr.optional();
		EXPECT_NO_THROW(sdlRealArr.fromSdl(owner, SdlInputClause("Wowowowow"), ctx));
		ASSERT_TRUE(owner.arr.size() == 1);
		PH_EXPECT_REAL_EQ(owner.arr[0], 0.0_r);

		sdlRealArr.defaultTo({-2.0_r, -1.0_r});
		sdlRealArr.niceToHave();
		EXPECT_NO_THROW(sdlRealArr.fromSdl(owner, SdlInputClause("testing"), ctx));
		ASSERT_TRUE(owner.arr.size() == 2);
		PH_EXPECT_REAL_EQ(owner.arr[0], -2.0_r);
		PH_EXPECT_REAL_EQ(owner.arr[1], -1.0_r);
	}

	// Disable fallback for fields
	{
		TSdlRealArray<RealArrOwner> sdlRealArr("array3", &RealArrOwner::arr);
		sdlRealArr.options({EFieldOption::DisableFallback});
		sdlRealArr.defaultTo({123.0_r, 456.0_r, 789.0_r});

		RealArrOwner owner;
		owner.arr = {6.7_r, -8.9_r};

		SdlInputContext ctx;

		EXPECT_THROW(sdlRealArr.fromSdl(owner, SdlInputClause("fltArrYoyo"), ctx), SdlLoadError);
		ASSERT_TRUE(owner.arr.size() == 2);
		PH_EXPECT_REAL_EQ(owner.arr[0],  6.7_r);// owner value should not update
		PH_EXPECT_REAL_EQ(owner.arr[1], -8.9_r);//
	}
}

TEST(TSdlRealArrayTest, NativeData)
{
	TSdlRealArray<RealArrOwner> sdlRealArr("array", &RealArrOwner::arr);
	RealArrOwner owner;
	owner.arr = {-1.1_r, 2.2_r, 3.3_r};

	SdlNativeData data = sdlRealArr.ownedNativeData(owner);
	EXPECT_EQ(data.numElements, 3);
	EXPECT_EQ(data.elementContainer, ESdlDataFormat::Vector);
	EXPECT_EQ(data.elementType, sdl::float_type_of<real>());

	ASSERT_TRUE(data);

	// Access by native data getter
	PH_EXPECT_REAL_EQ(data.get<float64>(0).value_or(0.0), -1.1_r);
	PH_EXPECT_REAL_EQ(data.get<float64>(1).value_or(0.0), 2.2_r);
	PH_EXPECT_REAL_EQ(data.get<float64>(2).value_or(0.0), 3.3_r);

	// Which is what native data actually stored
	PH_EXPECT_REAL_EQ(owner.arr[0], -1.1_r);
	PH_EXPECT_REAL_EQ(owner.arr[1], 2.2_r);
	PH_EXPECT_REAL_EQ(owner.arr[2], 3.3_r);

	// Modify `arr[1]` by native data setter
	EXPECT_TRUE(data.set<float64>(1, -22.2));

	// Access by native data getter again after update
	PH_EXPECT_REAL_EQ(data.get<float64>(0).value_or(0.0), -1.1_r);
	PH_EXPECT_REAL_EQ(data.get<float64>(1).value_or(0.0), -22.2_r);// <- changed by setter
	PH_EXPECT_REAL_EQ(data.get<float64>(2).value_or(0.0), 3.3_r);

	// Which is what native data actually stored
	PH_EXPECT_REAL_EQ(owner.arr[0], -1.1_r);
	PH_EXPECT_REAL_EQ(owner.arr[1], -22.2_r);// <- changed by setter
	PH_EXPECT_REAL_EQ(owner.arr[2], 3.3_r);
}

// TODO: ReadFromSdlInFile
