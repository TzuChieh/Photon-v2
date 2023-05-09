#include "util.h"

#include <SDL/Introspect/TSdlVector3Array.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

#include <vector>
#include <type_traits>

using namespace ph;

namespace
{
	class Vec3ArrOwner
	{
	public:
		std::vector<math::Vector3R> arr;
	};
}

TEST(TSdlVector3ArrayTest, RequiredProperties)
{
	{
		using FieldType = TSdlVector3Array<Vec3ArrOwner>;
		EXPECT_TRUE(std::is_copy_constructible_v<FieldType>);
		EXPECT_TRUE(std::is_move_constructible_v<FieldType>);
	}
}

TEST(TSdlVector3ArrayTest, ReadFromSdl)
{
	{
		TSdlVector3Array<Vec3ArrOwner> sdlVec3Arr("array", &Vec3ArrOwner::arr);

		// Default initialization performed on default value by default
		EXPECT_TRUE(sdlVec3Arr.defaultValue().empty());

		Vec3ArrOwner owner;
		SdlInputContext ctx;

		EXPECT_NO_THROW(sdlVec3Arr.fromSdl(owner, SdlInputClause("\"1.2   -3.4  10.0 \"  \" 2  3 \t5 \""), ctx));
		ASSERT_TRUE(owner.arr.size() == 2);
		PH_EXPECT_REAL_EQ(owner.arr[0].x(), 1.2_r);
		PH_EXPECT_REAL_EQ(owner.arr[0].y(), -3.4_r);
		PH_EXPECT_REAL_EQ(owner.arr[0].z(), 10.0_r);
		PH_EXPECT_REAL_EQ(owner.arr[1].x(), 2.0_r);
		PH_EXPECT_REAL_EQ(owner.arr[1].y(), 3.0_r);
		PH_EXPECT_REAL_EQ(owner.arr[1].z(), 5.0_r);

		// TODO: scientific notation

		// Fallback to default for optional & nice-to-have fields

		sdlVec3Arr.defaultTo({{-1.0_r, -2.0_r, -3.0_r}});
		sdlVec3Arr.optional();
		EXPECT_NO_THROW(sdlVec3Arr.fromSdl(owner, SdlInputClause("whatIsThis"), ctx));
		ASSERT_TRUE(owner.arr.size() == 1);
		PH_EXPECT_REAL_EQ(owner.arr[0].x(), -1.0_r);
		PH_EXPECT_REAL_EQ(owner.arr[0].y(), -2.0_r);
		PH_EXPECT_REAL_EQ(owner.arr[0].z(), -3.0_r);

		sdlVec3Arr.defaultTo({{-1.0_r, -2.0_r, -3.0_r}, {1.0_r, 2.0_r, 3.0_r}});
		sdlVec3Arr.niceToHave();
		EXPECT_NO_THROW(sdlVec3Arr.fromSdl(owner, SdlInputClause("testing"), ctx));
		ASSERT_TRUE(owner.arr.size() == 2);
		PH_EXPECT_REAL_EQ(owner.arr[0].x(), -1.0_r);
		PH_EXPECT_REAL_EQ(owner.arr[0].y(), -2.0_r);
		PH_EXPECT_REAL_EQ(owner.arr[0].z(), -3.0_r);
		PH_EXPECT_REAL_EQ(owner.arr[1].x(), 1.0_r);
		PH_EXPECT_REAL_EQ(owner.arr[1].y(), 2.0_r);
		PH_EXPECT_REAL_EQ(owner.arr[1].z(), 3.0_r);
	}

	// Disable fallback for fields
	{
		TSdlVector3Array<Vec3ArrOwner> sdlVec3Arr("arrayHa", &Vec3ArrOwner::arr);
		sdlVec3Arr.enableFallback(false);
		sdlVec3Arr.defaultTo({{123.0_r, 456.0_r, 789.0_r}});

		Vec3ArrOwner owner;
		owner.arr = {{1.1_r, 2.2_r, 3.3_r}};

		SdlInputContext ctx;

		EXPECT_THROW(sdlVec3Arr.fromSdl(owner, SdlInputClause("vec3ArrYoyo"), ctx), SdlLoadError);
		ASSERT_TRUE(owner.arr.size() == 1);
		PH_EXPECT_REAL_EQ(owner.arr[0].x(), 1.1_r);// owner value should not update
		PH_EXPECT_REAL_EQ(owner.arr[0].y(), 2.2_r);//
		PH_EXPECT_REAL_EQ(owner.arr[0].z(), 3.3_r);//
	}
}

// TODO: ReadFromSdlInFile
