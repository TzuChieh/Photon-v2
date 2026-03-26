#include <Common/memory.h>
#include <Common/compiler.h>
#include <Common/primitive_type.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstddef>
#include <bit>
#include <type_traits>

TEST(MemoryTest, AllocateAlignedMemory)
{
	// `sizeof(void*)` must be a multiple of 2 for the following loop to work
	ASSERT_TRUE(std::has_single_bit(sizeof(void*)));

	// Max alignment: 64 MiB
	for(std::size_t alignment = sizeof(void*); alignment <= 1024 * 1024 * 64; alignment *= 2)
	{
		// Max allocated size: 64 MiB * 4
		for(std::size_t size = alignment; size <= alignment * 4; size += alignment)
		{
			auto res = ph::make_aligned_memory(size, alignment);
			ASSERT_TRUE(res);

			auto address = reinterpret_cast<std::uintptr_t>(res.get());
			EXPECT_EQ(address % alignment, 0);

			//std::cerr << size << ", " << alignment << ", " << address << std::endl;
		}
	}
}
TEST(MemoryTest, AlignedArrayRequirements)
{
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<int, 10, 4>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<int, 10, 8>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<int, 10, 16>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<int, 10, 128>>);

	static_assert(std::is_standard_layout_v<ph::TAlignedArray<double, 8, 8>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<double, 9, 32>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<double, 10, 64>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<double, 10, 128>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<double, 11, 256>>);

	static_assert(std::is_standard_layout_v<ph::TAlignedArray<std::array<float, 4>, 9, 16>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<std::array<float, 5>, 10, 32>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<std::array<float, 5>, 10, 64>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<std::array<float, 5>, 10, 128>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<std::array<float, 5>, 10, 256>>);
	static_assert(std::is_standard_layout_v<ph::TAlignedArray<std::array<float, 5>, 10, 512>>);
}

TEST(MemoryTest, ToAndFromBytes)
{
	// Test with int32
	{
		const ph::int32 value = 0x12345678;
		std::byte bytes[sizeof(ph::int32)];
		ph::to_bytes(value, bytes);

		ph::int32 recovered;
		ph::from_bytes(bytes, &recovered);
		EXPECT_EQ(value, recovered);
	}

	// Test with float64
	{
		const ph::float64 value = 3.14159265358979323846;
		std::byte bytes[sizeof(ph::float64)];
		ph::to_bytes(value, bytes);

		ph::float64 recovered;
		ph::from_bytes(bytes, &recovered);
		EXPECT_DOUBLE_EQ(value, recovered);
	}

	// Test with a trivially copyable struct
	{
		struct TriviallyCopyable
		{
			int a;
			float b;
			char c;
		};
		static_assert(std::is_trivially_copyable_v<TriviallyCopyable>);

		const TriviallyCopyable value{42, 1.23f, 'z'};
		std::byte bytes[sizeof(TriviallyCopyable)];
		ph::to_bytes(value, bytes);

		TriviallyCopyable recovered;
		ph::from_bytes(bytes, &recovered);
		EXPECT_EQ(value.a, recovered.a);
		EXPECT_EQ(value.b, recovered.b);
		EXPECT_EQ(value.c, recovered.c);
	}
}

TEST(MemoryTest, ReverseBytes)
{
	// 1 byte (no change)
	{
		std::byte bytes[1] = {std::byte{0xAA}};
		ph::reverse_bytes<1>(bytes);
		EXPECT_EQ(bytes[0], std::byte{0xAA});
	}

	// 2 bytes (byteswap)
	{
		std::byte bytes[2] = {std::byte{0x11}, std::byte{0x22}};
		ph::reverse_bytes<2>(bytes);
		EXPECT_EQ(bytes[0], std::byte{0x22});
		EXPECT_EQ(bytes[1], std::byte{0x11});
	}

	// 4 bytes (byteswap)
	{
		std::byte bytes[4] = {std::byte{0x11}, std::byte{0x22}, std::byte{0x33}, std::byte{0x44}};
		ph::reverse_bytes<4>(bytes);
		EXPECT_EQ(bytes[0], std::byte{0x44});
		EXPECT_EQ(bytes[1], std::byte{0x33});
		EXPECT_EQ(bytes[2], std::byte{0x22});
		EXPECT_EQ(bytes[3], std::byte{0x11});
	}

	// 8 bytes (byteswap)
	{
		std::byte bytes[8] = {
			std::byte{0x11}, std::byte{0x22}, std::byte{0x33}, std::byte{0x44},
			std::byte{0x55}, std::byte{0x66}, std::byte{0x77}, std::byte{0x88}};
		ph::reverse_bytes<8>(bytes);
		EXPECT_EQ(bytes[0], std::byte{0x88});
		EXPECT_EQ(bytes[7], std::byte{0x11});
	}

	// 3 bytes (fallback)
	{
		std::byte bytes[3] = {std::byte{0x11}, std::byte{0x22}, std::byte{0x33}};
		ph::reverse_bytes<3>(bytes);
		EXPECT_EQ(bytes[0], std::byte{0x33});
		EXPECT_EQ(bytes[1], std::byte{0x22});
		EXPECT_EQ(bytes[2], std::byte{0x11});
	}
}
