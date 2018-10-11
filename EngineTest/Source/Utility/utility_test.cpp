#include <Utility/utility.h>

#include <gtest/gtest.h>

#include <type_traits>

TEST(UtilityTest, PointerAccess)
{
	class SomeClass
	{
	public: int x = 7;
	};

	SomeClass  value;
	SomeClass* pointer = &value;
	EXPECT_EQ(ph::pointer_access(value)->x,   7);
	EXPECT_EQ(ph::pointer_access(pointer)->x, 7);

	const SomeClass  cValue;
	const SomeClass* cPointer = &cValue;
	EXPECT_EQ(ph::pointer_access(cValue)->x,   7);
	EXPECT_EQ(ph::pointer_access(cPointer)->x, 7);

	const volatile SomeClass  cvValue;
	const volatile SomeClass* cvPointer = &cvValue;
	EXPECT_EQ(ph::pointer_access(cvValue)->x,   7);
	EXPECT_EQ(ph::pointer_access(cvPointer)->x, 7);
}

TEST(UtilityTest, RegularAccess)
{
	class SomeClass
	{
	public: int t = 3;
	};

	SomeClass  value;
	SomeClass* pointer = &value;
	EXPECT_EQ(ph::regular_access(SomeClass()).t,   3);
	EXPECT_EQ(ph::regular_access(pointer).t, 3);
	EXPECT_TRUE(std::is_reference_v<decltype(ph::regular_access(pointer))>);

	const SomeClass  cValue;
	const SomeClass* cPointer = &cValue;
	EXPECT_EQ(ph::regular_access(cValue).t,   3);
	EXPECT_EQ(ph::regular_access(cPointer).t, 3);
	EXPECT_TRUE(std::is_reference_v<decltype(ph::regular_access(cPointer))>);

	const volatile SomeClass  cvValue;
	const volatile SomeClass* cvPointer = &cvValue;
	EXPECT_EQ(ph::regular_access(cvValue).t,   3);
	EXPECT_EQ(ph::regular_access(cvPointer).t, 3);
	EXPECT_TRUE(std::is_reference_v<decltype(ph::regular_access(cvPointer))>);
}