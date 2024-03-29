#include "config.h"

#include <Math/Random/Pcg64DXSM.h>
#include <DataIO/Data/CsvFile.h>
#include <DataIO/FileSystem/Path.h>
#include <Common/primitive_type.h>

#include <gtest/gtest.h>

#include <vector>
#include <cstddef>
#include <limits>

using namespace ph;
using namespace ph::math;

TEST(Pcg64DXSMTest, Pcg64_128BitMath)
{
	using ph::math::detail::Pcg64UInt128;

	// Multiplying with 0
	{
		{
			auto x = Pcg64UInt128(0, 0) * Pcg64UInt128(0, 0);
			EXPECT_EQ(x.getHigh64(), 0); EXPECT_EQ(x.getLow64(), 0);
		}
		{
			auto x = Pcg64UInt128(0, 0) * Pcg64UInt128(0, 1234567);
			EXPECT_EQ(x.getHigh64(), 0); EXPECT_EQ(x.getLow64(), 0);
		}
		{
			auto x = Pcg64UInt128(7654321, 0) * Pcg64UInt128(0, 0);
			EXPECT_EQ(x.getHigh64(), 0); EXPECT_EQ(x.getLow64(), 0);
		}
	}

	// Multiplying with 1
	{
		{
			auto x = Pcg64UInt128(0, 1) * Pcg64UInt128(0, 1);
			EXPECT_EQ(x.getHigh64(), 0); EXPECT_EQ(x.getLow64(), 1);
		}
		{
			auto x = Pcg64UInt128(0, 16) * Pcg64UInt128(0, 1);
			EXPECT_EQ(x.getHigh64(), 0); EXPECT_EQ(x.getLow64(), 16);
		}
		{
			auto x = Pcg64UInt128(0, 1) * Pcg64UInt128(0, 65536);
			EXPECT_EQ(x.getHigh64(), 0); EXPECT_EQ(x.getLow64(), 65536);
		}
		{
			auto x = Pcg64UInt128(0, 1) * Pcg64UInt128(0, std::numeric_limits<uint64>::max());
			EXPECT_EQ(x.getHigh64(), 0); EXPECT_EQ(x.getLow64(), std::numeric_limits<uint64>::max());
		}
		{
			auto x = Pcg64UInt128(0, std::numeric_limits<uint64>::max()) * Pcg64UInt128(0, 1);
			EXPECT_EQ(x.getHigh64(), 0); EXPECT_EQ(x.getLow64(), std::numeric_limits<uint64>::max());
		}
	}

	// Multiplying with various large numbers (input <= 64 bits, result <= 128 bits)
	{
		{
			auto x = Pcg64UInt128(0, 0xFFFFFFFFull) * Pcg64UInt128(0, 0xFFFFFFFFull);
			EXPECT_EQ(x.getHigh64(), 0); EXPECT_EQ(x.getLow64(), 0xFFFFFFFE00000001ull);
		}
		{
			auto x = Pcg64UInt128(0, 0xFFFFFFFFFFull) * Pcg64UInt128(0, 0xFFFFFFFFFFull);
			EXPECT_EQ(x.getHigh64(), 0xFFFFull); EXPECT_EQ(x.getLow64(), 0xFFFFFE0000000001ull);
		}
		{
			auto x = Pcg64UInt128(0, 0xABCDABCDABCDull) * Pcg64UInt128(0, 0xABCDABCDABCDull);
			EXPECT_EQ(x.getHigh64(), 0x734C68C1ull); EXPECT_EQ(x.getLow64(), 0x5E356D12779D8229ull);
		}
	}

	// Multiplying with various large numbers (input <= 128 bits, result <= 128 bits)
	{
		{
			auto x = Pcg64UInt128(0xFFFFull, 0xAAAABBBBCCCCDDDDull) * 
			         Pcg64UInt128(0, 0xFFFFFFFFull);
			EXPECT_EQ(x.getHigh64(), 0xFFFFAAA9BBBCull); EXPECT_EQ(x.getLow64(), 0x2222222133332223ull);
		}
	}

	// Multiplying with various large numbers (input <= 128 bits, result > 128 bits and discarded)
	{
		{
			auto x = Pcg64UInt128(0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFFull) *
			         Pcg64UInt128(0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFFull);
			EXPECT_EQ(x.getHigh64(), 0x0000000000000000ull); EXPECT_EQ(x.getLow64(), 0x0000000000000001ull);
		}
		{
			auto x = Pcg64UInt128(0xAAAAAAAAAAAAAAAAull, 0xAAAAAAAAAAAAAAAAull) *
			         Pcg64UInt128(0xAAAAAAAAAAAAAAAAull, 0xAAAAAAAAAAAAAAAAull);
			EXPECT_EQ(x.getHigh64(), 0xE38E38E38E38E38Eull); EXPECT_EQ(x.getLow64(), 0x38E38E38E38E38E4ull);
		}
	}
}

TEST(Pcg64DXSMTest, GenerateRandomBits)
{
	// Sample values generated by Melissa O'Neill's `pcg-cpp`
	CsvFile sampleFile(Path(PH_TEST_RESOURCE_PATH("PCG_samples/cm_setseq_dxsm_128_64_state123_seq456_first2048.csv")));

	std::vector<uint64> expectedValues;
	const auto& row = sampleFile.getRow(0);
	for(std::size_t i = 0; i < row.numValues(); ++i)
	{
		expectedValues.push_back(row.getValue<uint64>(i));
	}

	ASSERT_EQ(expectedValues.size(), 2048);

	uint64 seq = 456;
	uint64 state = 123;
	Pcg64DXSM rng(0, seq, 0, state);
	for(std::size_t i = 0; i < 2048; ++i)
	{
		EXPECT_EQ(rng.generate(), expectedValues[i]);
	}
}

TEST(Pcg64DXSMTest, JumpAhead)
{
	// Use a reference rng to generate expected values
	std::vector<uint64> expectedValues;
	{
		Pcg64DXSM rng;
		for(std::size_t i = 0; i < 1024; ++i)
		{
			expectedValues.push_back(rng.generate());
		}
	}
	
	// Jump some distances and lookup to see if it is correct

	// 0-distance jump
	{
		Pcg64DXSM rng;
		rng.jumpAhead(0);
		EXPECT_EQ(rng.generate(), expectedValues[0]);
	}
	
	// Single jump
	{
		for(std::size_t distance = 1; distance < expectedValues.size(); ++distance)
		{
			Pcg64DXSM rng;
			rng.jumpAhead(distance);
			EXPECT_EQ(rng.generate(), expectedValues[distance]);
		}
	}

	// Multiple jumps
	{
		const uint64 distance1 = 12;
		const uint64 distance2 = 34;
		const uint64 distance3 = 56;

		Pcg64DXSM rng;
		rng.jumpAhead(distance1);
		rng.jumpAhead(distance2);
		rng.jumpAhead(distance3);

		EXPECT_EQ(rng.generate(), expectedValues[distance1 + distance2 + distance3]);
	}
}
