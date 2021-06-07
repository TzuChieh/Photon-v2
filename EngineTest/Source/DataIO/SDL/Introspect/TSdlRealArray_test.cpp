#include "util.h"

#include <DataIO/SDL/Introspect/TSdlRealArray.h>

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
		EXPECT_TRUE(sdlRealArr.getDefaultValue().empty());

		RealArrOwner owner;
		SdlInputContext ctx;

		EXPECT_NO_THROW(sdlRealArr.fromSdl(owner, "1.2    2   3.876 -456.789", ctx));
		ASSERT_TRUE(owner.arr.size() == 4);
		PH_EXPECT_REAL_EQ(owner.arr[0], 1.2_r);
		PH_EXPECT_REAL_EQ(owner.arr[1], 2.0_r);
		PH_EXPECT_REAL_EQ(owner.arr[2], 3.876_r);
		PH_EXPECT_REAL_EQ(owner.arr[3], -456.789_r);

		// TODO: scientific notation

		// Fallback to default for optional & nice-to-have fields

		sdlRealArr.defaultTo({0.0_r});
		sdlRealArr.withImportance(EFieldImportance::OPTIONAL);
		EXPECT_NO_THROW(sdlRealArr.fromSdl(owner, "Wowowowow", ctx));
		ASSERT_TRUE(owner.arr.size() == 1);
		PH_EXPECT_REAL_EQ(owner.arr[0], 0.0_r);

		sdlRealArr.defaultTo({-2.0_r, -1.0_r});
		sdlRealArr.withImportance(EFieldImportance::NICE_TO_HAVE);
		EXPECT_NO_THROW(sdlRealArr.fromSdl(owner, "testing", ctx));
		ASSERT_TRUE(owner.arr.size() == 2);
		PH_EXPECT_REAL_EQ(owner.arr[0], -2.0_r);
		PH_EXPECT_REAL_EQ(owner.arr[1], -1.0_r);
	}

	// Disable fallback for fields
	{
		TSdlRealArray<RealArrOwner> sdlRealArr("array3", &RealArrOwner::arr);
		sdlRealArr.enableFallback(false);
		sdlRealArr.defaultTo({123.0_r, 456.0_r, 789.0_r});

		RealArrOwner owner;
		owner.arr = {6.7_r, -8.9_r};

		SdlInputContext ctx;

		EXPECT_THROW(sdlRealArr.fromSdl(owner, "fltArrYoyo", ctx), SdlLoadError);
		ASSERT_TRUE(owner.arr.size() == 2);
		PH_EXPECT_REAL_EQ(owner.arr[0],  6.7_r);// owner value should not update
		PH_EXPECT_REAL_EQ(owner.arr[1], -8.9_r);//
	}
}

// TODO: ReadFromSdlInFile
