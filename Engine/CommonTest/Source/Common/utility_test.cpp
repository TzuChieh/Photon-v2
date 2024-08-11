#include <Common/utility.h>
#include <Common/exceptions.h>

#include <gtest/gtest.h>

#include <type_traits>
#include <cstdint>
#include <limits>
#include <array>

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

TEST(UtilityTest, MakeArray)
{
	// Make an array filled with specified value
	{
		const auto arr = ph::make_array<int, 5>(77);
		ASSERT_EQ(arr.size(), 5);
		EXPECT_EQ(arr[0], 77);
		EXPECT_EQ(arr[1], 77);
		EXPECT_EQ(arr[2], 77);
		EXPECT_EQ(arr[3], 77);
		EXPECT_EQ(arr[4], 77);
	}

	// Make an array filled with a non default-constructible value
	{
		struct TestType
		{
			int x = 0;

			TestType() = delete;

			explicit TestType(const int x)
				: x(x)
			{}
		};
		
		const auto arr = ph::make_array<TestType, 3>(TestType{6});
		ASSERT_EQ(arr.size(), 3);
		EXPECT_EQ(arr[0].x, 6);
		EXPECT_EQ(arr[1].x, 6);
		EXPECT_EQ(arr[2].x, 6);
	}
}
