#include <DataIO/SDL/Introspect/TSdlVector3.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

#include <string>
#include <type_traits>

using namespace ph;
using namespace ph::math;

namespace
{
	class VecOwner
	{
	public:
		Vector3R v;
	};
}

TEST(TSdlVector3Test, RequiredProperties)
{
	{
		using FieldType = TSdlVector3<VecOwner>;
		EXPECT_TRUE(std::is_copy_constructible_v<FieldType>);
		EXPECT_TRUE(std::is_move_constructible_v<FieldType>);
	}
}

TEST(TSdlVector3Test, ReadFromSdl)
{
	{
		TSdlVector3<VecOwner> sdlVec("dir", &VecOwner::v);

		// By default the default value is zero vector
		EXPECT_EQ(sdlVec.defaultValue(), Vector3R(0, 0, 0));

		sdlVec.defaultTo(Vector3R(0, 1, 0));
		EXPECT_EQ(sdlVec.defaultValue(), Vector3R(0, 1, 0));

		VecOwner owner;
		owner.v = Vector3R(0);

		SdlInputContext ctx;

		EXPECT_NO_THROW(sdlVec.fromSdl(owner, SdlInputPayload("1 2 3"), ctx));

		EXPECT_EQ(owner.v, Vector3R(1, 2, 3));

		// Spaces in SDL string are skipped
		EXPECT_NO_THROW(sdlVec.fromSdl(owner, SdlInputPayload("  1 1   1 "), ctx));
		EXPECT_EQ(owner.v, Vector3R(1, 1, 1));

		// Read string with different importance

		sdlVec.optional();
		EXPECT_NO_THROW(sdlVec.fromSdl(owner, SdlInputPayload("0 0 1"), ctx));
		EXPECT_EQ(owner.v, Vector3R(0, 0, 1));

		sdlVec.niceToHave();
		EXPECT_NO_THROW(sdlVec.fromSdl(owner, SdlInputPayload("-1 -1 -1"), ctx));
		EXPECT_EQ(owner.v, Vector3R(-1, -1, -1));

		sdlVec.required();
		EXPECT_NO_THROW(sdlVec.fromSdl(owner, SdlInputPayload("100 100 -200"), ctx));
		EXPECT_EQ(owner.v, Vector3R(100, 100, -200));
	}
}
