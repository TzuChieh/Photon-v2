#include <DataIO/SDL/Introspect/TSdlQuaternion.h>
#include <Math/TQuaternion.h>

#include <gtest/gtest.h>

#include <string>
#include <type_traits>

using namespace ph;
using namespace ph::math;

namespace
{

class QuatOwner
{
public:
	QuaternionR q;
};

}

TEST(TSdlQuaternionTest, RequiredProperties)
{
	{
		using FieldType = TSdlQuaternion<QuatOwner>;
		EXPECT_TRUE(std::is_copy_constructible_v<FieldType>);
		EXPECT_TRUE(std::is_move_constructible_v<FieldType>);
	}
}

TEST(TSdlQuaternionTest, ReadFromSdl)
{
	{
		TSdlQuaternion<QuatOwner> sdlQuat("rrr", &QuatOwner::q);

		// By default the default value is a no-rotation one
		EXPECT_EQ(sdlQuat.defaultValue(), QuaternionR::makeNoRotation());

		sdlQuat.defaultTo(QuaternionR(0, 1, 0, 1));
		EXPECT_EQ(sdlQuat.defaultValue(), QuaternionR(0, 1, 0, 1));

		QuatOwner owner;
		owner.q = QuaternionR::makeNoRotation();

		SdlInputContext ctx;

		EXPECT_NO_THROW(sdlQuat.fromSdl(owner, SdlInputPayload("1 2 3 4"), ctx));
		EXPECT_EQ(owner.q, QuaternionR(1, 2, 3, 4));

		// Spaces in SDL string are skipped
		EXPECT_NO_THROW(sdlQuat.fromSdl(owner, SdlInputPayload("  2  2 2  2 "), ctx));
		EXPECT_EQ(owner.q, QuaternionR(2, 2, 2, 2));

		// Read string with different importance

		sdlQuat.withImportance(EFieldImportance::Optional);
		EXPECT_NO_THROW(sdlQuat.fromSdl(owner, SdlInputPayload("0 0 1 0"), ctx));
		EXPECT_EQ(owner.q, QuaternionR(0, 0, 1, 0));

		sdlQuat.withImportance(EFieldImportance::NiceToHave);
		EXPECT_NO_THROW(sdlQuat.fromSdl(owner, SdlInputPayload("-1 -1 -1 -1"), ctx));
		EXPECT_EQ(owner.q, QuaternionR(-1, -1, -1, -1));

		sdlQuat.withImportance(EFieldImportance::Required);
		EXPECT_NO_THROW(sdlQuat.fromSdl(owner, SdlInputPayload("100 100 -200 300"), ctx));
		EXPECT_EQ(owner.q, QuaternionR(100, 100, -200, 300));
	}
}
