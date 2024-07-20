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
void test_lossless_cast(std::type_identity_t<SrcType> src)
{
	const auto expectedDst = static_cast<DstType>(src);
	EXPECT_EQ(expectedDst, ph::lossless_cast<DstType>(src));
}

}

TEST(UtilityTest, LosslessIntegerIntegerCast)
{
	// signed -> signed
	test_lossless_cast<std::int64_t, std::int32_t>(0);
	test_lossless_cast<std::int64_t, std::int32_t>(-1234);
	test_lossless_cast<std::int64_t, std::int32_t>(56789);
	test_lossless_cast<std::int64_t, std::int32_t>(std::numeric_limits<std::int32_t>::max());
	test_lossless_cast<std::int64_t, std::int32_t>(std::numeric_limits<std::int32_t>::lowest());
	test_lossless_cast<std::int16_t, std::int32_t>(-2785);
	test_lossless_cast<std::int8_t, std::int16_t>(-66);
	test_lossless_cast<std::int8_t, std::int8_t>(std::numeric_limits<std::int8_t>::lowest());
	EXPECT_THROW((test_lossless_cast<std::int64_t, std::int32_t>(static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::max()) + 1)), ph::OverflowException);
	EXPECT_THROW((test_lossless_cast<std::int64_t, std::int32_t>(std::numeric_limits<std::int64_t>::max())), ph::OverflowException);
	EXPECT_THROW((test_lossless_cast<std::int64_t, std::int32_t>(static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::lowest()) - 1)), ph::OverflowException);
	EXPECT_THROW((test_lossless_cast<std::int64_t, std::int32_t>(std::numeric_limits<std::int64_t>::lowest())), ph::OverflowException);

	// signed -> unsigned
	test_lossless_cast<std::int64_t, std::uint32_t>(0);
	test_lossless_cast<std::int64_t, std::uint32_t>(7777);
	test_lossless_cast<std::int64_t, std::uint32_t>(22222);
	test_lossless_cast<std::int64_t, std::uint32_t>(std::numeric_limits<std::uint32_t>::max());
	test_lossless_cast<std::int64_t, std::uint32_t>(std::numeric_limits<std::uint32_t>::lowest());
	EXPECT_THROW((test_lossless_cast<std::int64_t, std::uint32_t>(static_cast<std::int64_t>(std::numeric_limits<std::uint32_t>::max()) + 1)), ph::OverflowException);
	EXPECT_THROW((test_lossless_cast<std::int64_t, std::uint32_t>(std::numeric_limits<std::int64_t>::max())), ph::OverflowException);
	EXPECT_THROW((test_lossless_cast<std::int64_t, std::uint32_t>(-1)), ph::OverflowException);
	EXPECT_THROW((test_lossless_cast<std::int64_t, std::uint32_t>(-33333)), ph::OverflowException);
	EXPECT_THROW((test_lossless_cast<std::int64_t, std::uint32_t>(std::numeric_limits<std::int64_t>::lowest())), ph::OverflowException);

	// usigned -> signed
	test_lossless_cast<std::uint64_t, std::int32_t>(0);
	test_lossless_cast<std::uint64_t, std::int32_t>(56789);
	test_lossless_cast<std::uint64_t, std::int32_t>(std::numeric_limits<std::int32_t>::max());
	test_lossless_cast<std::uint16_t, std::int32_t>(2785);
	test_lossless_cast<std::uint8_t, std::int16_t>(240);
	test_lossless_cast<std::uint8_t, std::int8_t>(std::numeric_limits<std::int8_t>::max());
	EXPECT_THROW((test_lossless_cast<std::uint64_t, std::int32_t>(static_cast<std::uint64_t>(std::numeric_limits<std::int32_t>::max()) + 1)), ph::OverflowException);
	EXPECT_THROW((test_lossless_cast<std::uint64_t, std::int32_t>(std::numeric_limits<std::uint64_t>::max())), ph::OverflowException);
	EXPECT_THROW((test_lossless_cast<std::uint32_t, std::int8_t>(static_cast<std::uint32_t>(std::numeric_limits<std::int8_t>::max()) + 1)), ph::OverflowException);
	EXPECT_THROW((test_lossless_cast<std::uint16_t, std::int8_t>(static_cast<std::uint16_t>(std::numeric_limits<std::int8_t>::max()) + 1)), ph::OverflowException);

	// usigned -> usigned
	test_lossless_cast<std::uint64_t, std::uint32_t>(0);
	test_lossless_cast<std::uint64_t, std::uint32_t>(56789);
	test_lossless_cast<std::uint64_t, std::uint32_t>(std::numeric_limits<std::uint32_t>::max());
	test_lossless_cast<std::uint16_t, std::uint32_t>(2785);
	test_lossless_cast<std::uint8_t, std::uint16_t>(240);
	test_lossless_cast<std::uint8_t, std::uint8_t>(std::numeric_limits<std::uint8_t>::max());
	EXPECT_THROW((test_lossless_cast<std::uint64_t, std::uint32_t>(static_cast<std::uint64_t>(std::numeric_limits<std::uint32_t>::max()) + 1)), ph::OverflowException);
	EXPECT_THROW((test_lossless_cast<std::uint64_t, std::uint32_t>(std::numeric_limits<std::uint64_t>::max())), ph::OverflowException);
	EXPECT_THROW((test_lossless_cast<std::uint32_t, std::uint8_t>(static_cast<std::uint32_t>(std::numeric_limits<std::uint8_t>::max()) + 1)), ph::OverflowException);
	EXPECT_THROW((test_lossless_cast<std::uint16_t, std::uint8_t>(static_cast<std::uint16_t>(std::numeric_limits<std::uint8_t>::max()) + 1)), ph::OverflowException);
}

TEST(UtilityTest, LosslessFloatFloatCast)
{
	// float -> float and double -> double, should always success on normal values
	test_lossless_cast<float, float>(0.0f);
	test_lossless_cast<float, float>(0.12345f);
	test_lossless_cast<float, float>(-76543.21f);
	test_lossless_cast<float, float>(std::numeric_limits<float>::max());
	test_lossless_cast<double, double>(0.0);
	test_lossless_cast<double, double>(0.987654321);
	test_lossless_cast<double, double>(123456.78);
	test_lossless_cast<double, double>(std::numeric_limits<double>::max());

	// float -> double, should always success
	test_lossless_cast<float, double>(0.0f);
	test_lossless_cast<float, double>(77777.7f);
	test_lossless_cast<float, double>(-999999999.9f);
	test_lossless_cast<float, double>(123.456f);
	test_lossless_cast<float, double>(std::numeric_limits<float>::max());
	test_lossless_cast<float, double>(std::numeric_limits<float>::min());
	test_lossless_cast<float, double>(std::numeric_limits<float>::lowest());

	// double -> float
	test_lossless_cast<double, float>(0.0);
	test_lossless_cast<double, float>(1.0);
	test_lossless_cast<double, float>(2.0);
	test_lossless_cast<double, float>(32768.0);
	test_lossless_cast<double, float>(0.5);
	test_lossless_cast<double, float>(0.25);
	test_lossless_cast<double, float>(0.125);
	EXPECT_THROW((test_lossless_cast<double, float>(123.45678910111213141516)), ph::NumericException);
	EXPECT_THROW((test_lossless_cast<double, float>(std::numeric_limits<double>::max())), ph::NumericException);
	EXPECT_THROW((test_lossless_cast<double, float>(std::numeric_limits<double>::min())), ph::NumericException);
	EXPECT_THROW((test_lossless_cast<double, float>(std::numeric_limits<double>::lowest())), ph::NumericException);
}
