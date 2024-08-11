#include <Utility/utility.h>

#include <gtest/gtest.h>

#include <type_traits>
#include <cstdint>
#include <limits>
#include <array>
#include <memory>

TEST(UtilityTest, PointerAccess)
{
	class SomeClass
	{
	public: int x = 7;
	};

	SomeClass  value;
	SomeClass* pointer = &value;
	EXPECT_EQ(ph::ptr_access(value)->x,   7);
	EXPECT_EQ(ph::ptr_access(pointer)->x, 7);

	const SomeClass  cValue;
	const SomeClass* cPointer = &cValue;
	EXPECT_EQ(ph::ptr_access(cValue)->x,   7);
	EXPECT_EQ(ph::ptr_access(cPointer)->x, 7);

	const volatile SomeClass  cvValue;
	const volatile SomeClass* cvPointer = &cvValue;
	EXPECT_EQ(ph::ptr_access(cvValue)->x,   7);
	EXPECT_EQ(ph::ptr_access(cvPointer)->x, 7);

	// Do not access managed data for smart pointers
	{
		auto sharedInt = std::make_shared<int>(123);

		EXPECT_EQ(*ph::ptr_access(sharedInt), sharedInt);
		EXPECT_EQ(**ph::ptr_access(sharedInt), 123);
		EXPECT_TRUE(std::is_pointer_v<decltype(ph::ptr_access(sharedInt))>);
	}

	// Do not access managed data for smart pointers
	{
		auto uniqueFloat = std::make_unique<float>(1.0f);

		EXPECT_EQ(*ph::ptr_access(uniqueFloat), uniqueFloat);
		EXPECT_EQ(**ph::ptr_access(uniqueFloat), 1.0f);
		EXPECT_TRUE(std::is_pointer_v<decltype(ph::ptr_access(uniqueFloat))>);
	}
}

TEST(UtilityTest, ReferenceAccess)
{
	class SomeClass
	{
	public: int t = 3;
	};

	SomeClass  value;
	SomeClass* pointer = &value;
	EXPECT_EQ(ph::ref_access(value).t,   3);
	EXPECT_EQ(ph::ref_access(pointer).t, 3);
	EXPECT_TRUE(std::is_reference_v<decltype(ph::ref_access(pointer))>);

	const SomeClass  cValue;
	const SomeClass* cPointer = &cValue;
	EXPECT_EQ(ph::ref_access(cValue).t,   3);
	EXPECT_EQ(ph::ref_access(cPointer).t, 3);
	EXPECT_TRUE(std::is_reference_v<decltype(ph::ref_access(cPointer))>);

	const volatile SomeClass  cvValue;
	const volatile SomeClass* cvPointer = &cvValue;
	EXPECT_EQ(ph::ref_access(cvValue).t,   3);
	EXPECT_EQ(ph::ref_access(cvPointer).t, 3);
	EXPECT_TRUE(std::is_reference_v<decltype(ph::ref_access(cvPointer))>);

	// Do not access managed data for smart pointers
	{
		auto sharedInt = std::make_shared<int>(123);

		EXPECT_EQ(ph::ref_access(sharedInt), sharedInt);
		EXPECT_EQ(*ph::ref_access(sharedInt), 123);
		EXPECT_TRUE(std::is_reference_v<decltype(ph::ref_access(sharedInt))>);
	}

	// Do not access managed data for smart pointers
	{
		auto uniqueFloat = std::make_unique<float>(1.0f);

		EXPECT_EQ(ph::ref_access(uniqueFloat), uniqueFloat);
		EXPECT_EQ(*ph::ref_access(uniqueFloat), 1.0f);
		EXPECT_TRUE(std::is_reference_v<decltype(ph::ref_access(uniqueFloat))>);
	}
}

TEST(UtilityTest, BitwiseCast)
{
	const std::int32_t source1 = 777;
	const auto target1 = ph::bitwise_cast<float>(source1);
	const auto result1 = ph::bitwise_cast<std::int32_t>(target1);
	EXPECT_NE(source1, target1);
	EXPECT_EQ(source1, result1);

	const auto source2 = std::numeric_limits<std::uint16_t>::max();
	const auto target2 = ph::bitwise_cast<std::int16_t>(source2);
	const auto result2 = ph::bitwise_cast<std::uint16_t>(target2);
	EXPECT_NE(source2, target2);
	EXPECT_EQ(source2, result2);

	const std::int64_t source3 = 123;
	const auto target3 = ph::bitwise_cast<double>(source3);
	const auto result3 = ph::bitwise_cast<std::int64_t>(target3);
	EXPECT_NE(source3, target3);
	EXPECT_EQ(source3, result3);
}
