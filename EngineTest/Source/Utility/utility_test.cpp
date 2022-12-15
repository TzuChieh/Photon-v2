#include <Utility/utility.h>

#include <gtest/gtest.h>

#include <type_traits>
#include <cstdint>
#include <limits>
#include <array>

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
	EXPECT_EQ(ph::regular_access(value).t,   3);
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

TEST(UtilityTest, BitwiseCast)
{
	const std::int32_t source1 = 777;
	const float        target1 = ph::bitwise_cast<std::int32_t, float>(source1);
	const std::int32_t result1 = ph::bitwise_cast<float, std::int32_t>(target1);
	EXPECT_NE(source1, target1);
	EXPECT_EQ(source1, result1);

	const std::uint16_t source2 = std::numeric_limits<std::uint16_t>::max();
	const std::int16_t  target2 = ph::bitwise_cast<std::uint16_t, std::int16_t>(source2);
	const std::uint16_t result2 = ph::bitwise_cast<std::int16_t, std::uint16_t>(target2);
	EXPECT_NE(source2, target2);
	EXPECT_EQ(source2, result2);

	const std::int64_t source3 = 123;
	const double       target3 = ph::bitwise_cast<std::int64_t, double>(source3);
	const std::int64_t result3 = ph::bitwise_cast<double, std::int64_t>(target3);
	EXPECT_NE(source3, target3);
	EXPECT_EQ(source3, result3);
}

TEST(UtilityTest, SizeofInBits)
{
	// sizeof types
	{
		EXPECT_EQ(ph::sizeof_in_bits<std::int8_t>(), 8);
		EXPECT_EQ(ph::sizeof_in_bits<std::uint16_t>(), 16);
		EXPECT_EQ(ph::sizeof_in_bits<std::uint32_t>(), 32);
		EXPECT_EQ(ph::sizeof_in_bits<std::int64_t>(), 64);
		EXPECT_EQ(ph::sizeof_in_bits<std::uint64_t>(), 64);

		using ArrType = std::array<int, 100>;
		EXPECT_GE(ph::sizeof_in_bits<ArrType>(), 16 * 100);
	}
}

namespace
{

template<typename SrcType, typename DstType>
void test_safe_number_cast(std::type_identity_t<SrcType> src)
{
	const auto expectedDst = static_cast<DstType>(src);
	EXPECT_EQ(expectedDst, ph::safe_number_cast<DstType>(src));
}

}

TEST(UtilityTest, SafeNumberCast)
{
	// Integer cast
	{
		// signed -> signed
		test_safe_number_cast<std::int64_t, std::int32_t>(0);
		test_safe_number_cast<std::int64_t, std::int32_t>(-1234);
		test_safe_number_cast<std::int64_t, std::int32_t>(56789);
		test_safe_number_cast<std::int64_t, std::int32_t>(std::numeric_limits<std::int32_t>::max());
		test_safe_number_cast<std::int64_t, std::int32_t>(std::numeric_limits<std::int32_t>::lowest());
		test_safe_number_cast<std::int16_t, std::int32_t>(-2785);
		test_safe_number_cast<std::int8_t, std::int16_t>(-66);
		test_safe_number_cast<std::int8_t, std::int8_t>(std::numeric_limits<std::int8_t>::lowest());
		EXPECT_THROW((test_safe_number_cast<std::int64_t, std::int32_t>(static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::max()) + 1)), ph::OverflowException);
		EXPECT_THROW((test_safe_number_cast<std::int64_t, std::int32_t>(std::numeric_limits<std::int64_t>::max())), ph::OverflowException);
		EXPECT_THROW((test_safe_number_cast<std::int64_t, std::int32_t>(static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::lowest()) - 1)), ph::OverflowException);
		EXPECT_THROW((test_safe_number_cast<std::int64_t, std::int32_t>(std::numeric_limits<std::int64_t>::lowest())), ph::OverflowException);

		// signed -> unsigned
		test_safe_number_cast<std::int64_t, std::uint32_t>(0);
		test_safe_number_cast<std::int64_t, std::uint32_t>(7777);
		test_safe_number_cast<std::int64_t, std::uint32_t>(22222);
		test_safe_number_cast<std::int64_t, std::uint32_t>(std::numeric_limits<std::uint32_t>::max());
		test_safe_number_cast<std::int64_t, std::uint32_t>(std::numeric_limits<std::uint32_t>::lowest());
		EXPECT_THROW((test_safe_number_cast<std::int64_t, std::uint32_t>(static_cast<std::int64_t>(std::numeric_limits<std::uint32_t>::max()) + 1)), ph::OverflowException);
		EXPECT_THROW((test_safe_number_cast<std::int64_t, std::uint32_t>(std::numeric_limits<std::int64_t>::max())), ph::OverflowException);
		EXPECT_THROW((test_safe_number_cast<std::int64_t, std::uint32_t>(-1)), ph::OverflowException);
		EXPECT_THROW((test_safe_number_cast<std::int64_t, std::uint32_t>(-33333)), ph::OverflowException);
		EXPECT_THROW((test_safe_number_cast<std::int64_t, std::uint32_t>(std::numeric_limits<std::int64_t>::lowest())), ph::OverflowException);

		// usigned -> signed
		test_safe_number_cast<std::uint64_t, std::int32_t>(0);
		test_safe_number_cast<std::uint64_t, std::int32_t>(56789);
		test_safe_number_cast<std::uint64_t, std::int32_t>(std::numeric_limits<std::int32_t>::max());
		test_safe_number_cast<std::uint16_t, std::int32_t>(2785);
		test_safe_number_cast<std::uint8_t, std::int16_t>(240);
		test_safe_number_cast<std::uint8_t, std::int8_t>(std::numeric_limits<std::int8_t>::max());
		EXPECT_THROW((test_safe_number_cast<std::uint64_t, std::int32_t>(static_cast<std::uint64_t>(std::numeric_limits<std::int32_t>::max()) + 1)), ph::OverflowException);
		EXPECT_THROW((test_safe_number_cast<std::uint64_t, std::int32_t>(std::numeric_limits<std::uint64_t>::max())), ph::OverflowException);
		EXPECT_THROW((test_safe_number_cast<std::uint32_t, std::int8_t>(static_cast<std::uint32_t>(std::numeric_limits<std::int8_t>::max()) + 1)), ph::OverflowException);
		EXPECT_THROW((test_safe_number_cast<std::uint16_t, std::int8_t>(static_cast<std::uint16_t>(std::numeric_limits<std::int8_t>::max()) + 1)), ph::OverflowException);

		// usigned -> usigned
		test_safe_number_cast<std::uint64_t, std::uint32_t>(0);
		test_safe_number_cast<std::uint64_t, std::uint32_t>(56789);
		test_safe_number_cast<std::uint64_t, std::uint32_t>(std::numeric_limits<std::uint32_t>::max());
		test_safe_number_cast<std::uint16_t, std::uint32_t>(2785);
		test_safe_number_cast<std::uint8_t, std::uint16_t>(240);
		test_safe_number_cast<std::uint8_t, std::uint8_t>(std::numeric_limits<std::uint8_t>::max());
		EXPECT_THROW((test_safe_number_cast<std::uint64_t, std::uint32_t>(static_cast<std::uint64_t>(std::numeric_limits<std::uint32_t>::max()) + 1)), ph::OverflowException);
		EXPECT_THROW((test_safe_number_cast<std::uint64_t, std::uint32_t>(std::numeric_limits<std::uint64_t>::max())), ph::OverflowException);
		EXPECT_THROW((test_safe_number_cast<std::uint32_t, std::uint8_t>(static_cast<std::uint32_t>(std::numeric_limits<std::uint8_t>::max()) + 1)), ph::OverflowException);
		EXPECT_THROW((test_safe_number_cast<std::uint16_t, std::uint8_t>(static_cast<std::uint16_t>(std::numeric_limits<std::uint8_t>::max()) + 1)), ph::OverflowException);
	}
}
