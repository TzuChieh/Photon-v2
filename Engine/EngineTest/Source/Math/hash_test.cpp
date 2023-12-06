#include <Math/hash.h>
#include <Common/primitive_type.h>

#include <gtest/gtest.h>

#include <vector>
#include <limits>
#include <cstddef>
#include <string>

using namespace ph;
using namespace ph::math;

TEST(HashTest, MurmurHash3_32_Test)
{
	auto test_string = 
		[](const std::string& s, uint32 seed, uint32 expected)
		{
			uint32 hash = murmur3_32(s.c_str(), s.length(), seed);
			EXPECT_EQ(hash, expected);
		};

	auto test_array = 
		[](const std::vector<uint8>& s, uint32 seed, uint32 expected)
		{
			uint32 hash = murmur3_32(s.data(), s.size(), seed);
			EXPECT_EQ(hash, expected);
		};

	// Test cases are from `vladimirgamalyan`'s test case.

	// With zero data and zero seed, everything becomes zero
	test_array({}, 0, 0);

	// Ignores nearly all the math
	test_array({}, 1, 0x514E28B7);

	// Make sure your seed uses unsigned 32-bit math
	test_array({}, 0xFFFFFFFF, 0x81F16F39);

	// Make sure 4-byte blocks use unsigned math
	test_array({0xFF, 0xFF, 0xFF, 0xFF}, 0, 0x76293B50);

	// Endian order. A `uint32` block should end up as 0x87654321
	test_array({0x21, 0x43, 0x65, 0x87}, 0, 0xF55B516B);	

	// Special seed value eliminates initial key with xor
	test_array({0x21, 0x43, 0x65, 0x87}, 0x5082EDEE, 0x2362F9DE);

	// Only three bytes. Should end up as 0x654321
	test_array({0x21, 0x43, 0x65}, 0, 0x7E4A8634);

	// Only two bytes. Should end up as 0x4321
	test_array({0x21, 0x43}, 0, 0xA0F7B07A);

	// Only one byte. Should end up as 0x21
	test_array({0x21}, 0, 0x72661CF4);

	// Make sure compiler doesn't see zero and convert to null
	test_array({0x00, 0x00, 0x00, 0x00}, 0, 0x2362F9DE);

	test_array({0x00, 0x00, 0x00}, 0, 0x85F0B427);
	test_array({0x00, 0x00}, 0, 0x30F4C306);
	test_array({0x00}, 0, 0x514E28B7);

	// Empty string with zero seed should give zero
	test_string("", 0, 0);

	test_string("", 1, 0x514E28B7);

	// Make sure seed value is handled unsigned
	test_string("", 0xFFFFFFFF, 0x81F16F39);

	// One full block
	test_string("aaaa", 0x9747B28C, 0x5A97808A);

	// Three characters
	test_string("aaa", 0x9747B28C, 0x283E0130);

	// Two characters
	test_string("aa", 0x9747B28C, 0x5D211726);

	// One character
	test_string("a", 0x9747B28C, 0x7FA09EA6);

	// Endian order within the blocks
	test_string("abcd", 0x9747B28C, 0xF0478627);
	test_string("abc", 0x9747B28C, 0xC84A62DD);
	test_string("ab", 0x9747B28C, 0x74875592);
	test_string("a", 0x9747B28C, 0x7FA09EA6);

	test_string("Hello, world!", 0x9747B28C, 0x24884CBA);

	test_string(
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 
		0x9747B28C, 
		0x37405BDC);

	test_string("abc", 0, 0xB3DD93FA);
	test_string("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 0, 0xEE925B90);
	test_string("The quick brown fox jumps over the lazy dog", 0x9747B28C, 0x2FA826CD);
}

//TEST(HashTest, MurmurHash3_32_CollisionTest)
//{
//	std::size_t numCollisions = 0;
//
//	std::vector<bool> isHashUsed(std::numeric_limits<uint32>::max(), false);
//	for(uint32 value = 0; value < isHashUsed.size(); ++value)
//	{
//		const auto hash = static_cast<uint32>(murmur3_32(&value, 1, 42));
//		/*ASSERT_FALSE(isHashUsed[hash]) <<
//			"(hash collision found: value = " << value << ", hash = " << hash << ")";*/
//	
//		if(isHashUsed[hash])
//		{
//			++numCollisions;
//		}
//	
//		isHashUsed[hash] = true;
//	}
//	
//	EXPECT_EQ(numCollisions, 0);
//}
