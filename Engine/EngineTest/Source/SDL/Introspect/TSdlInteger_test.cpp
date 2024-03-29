#include <SDL/Introspect/TSdlInteger.h>

#include <gtest/gtest.h>

#include <type_traits>

using namespace ph;

namespace
{
	class IntegerOwner
	{
	public:
		integer value;
	};
}

TEST(TSdlIntegerTest, RequiredProperties)
{
	{
		using FieldType = TSdlInteger<IntegerOwner, integer>;
		EXPECT_TRUE(std::is_copy_constructible_v<FieldType>);
		EXPECT_TRUE(std::is_move_constructible_v<FieldType>);
	}
}

TEST(TSdlIntegerTest, ReadFromSdl)
{
	{
		TSdlInteger<IntegerOwner, integer> sdlInt("number", &IntegerOwner::value);

		// Zero initialization performed on default value by default
		ASSERT_TRUE(sdlInt.getDefaultValue());
		EXPECT_EQ(*sdlInt.getDefaultValue(), 0);

		IntegerOwner owner;
		owner.value = 666;

		SdlInputContext ctx;

		EXPECT_NO_THROW(sdlInt.fromSdl(owner, SdlInputClause("123"), ctx));
		EXPECT_EQ(owner.value, 123);

		// Value with spaces
		EXPECT_NO_THROW(sdlInt.fromSdl(owner, SdlInputClause("  -7 "), ctx));
		EXPECT_EQ(owner.value, -7);

		// TODO: scientific notation

		// Fallback to default for fields with different importance

		sdlInt.defaultTo(12);
		sdlInt.withImportance(EFieldImportance::Optional);
		EXPECT_NO_THROW(sdlInt.fromSdl(owner, SdlInputClause("what"), ctx));
		EXPECT_EQ(owner.value, 12);

		sdlInt.defaultTo(333);
		sdlInt.withImportance(EFieldImportance::NiceToHave);
		EXPECT_NO_THROW(sdlInt.fromSdl(owner, SdlInputClause("not an int"), ctx));
		EXPECT_EQ(owner.value, 333);

		sdlInt.defaultTo(444);
		sdlInt.withImportance(EFieldImportance::Required);
		EXPECT_NO_THROW(sdlInt.fromSdl(owner, SdlInputClause("testing"), ctx));
		EXPECT_EQ(owner.value, 444);
	}

	// Disable fallback for fields
	{
		TSdlInteger<IntegerOwner, integer> sdlInt("number", &IntegerOwner::value);
		sdlInt.enableFallback(false);
		sdlInt.defaultTo(22);

		IntegerOwner owner;
		owner.value = 1;

		SdlInputContext ctx;

		EXPECT_THROW(sdlInt.fromSdl(owner, SdlInputClause("hello"), ctx), SdlLoadError);
		EXPECT_NE(owner.value, 22);// owner value should not update
	}
}
