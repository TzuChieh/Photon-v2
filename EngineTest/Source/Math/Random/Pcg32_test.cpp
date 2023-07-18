#include "config.h"

#include <Math/Random/Pcg32.h>
#include <DataIO/Data/CsvFile.h>
#include <DataIO/FileSystem/Path.h>
#include <Common/primitive_type.h>

#include <gtest/gtest.h>

#include <vector>
#include <cstddef>

using namespace ph;
using namespace ph::math;

TEST(Pcg32Test, GenerateRandomBits)
{
	CsvFile sampleFile(Path(PH_TEST_RESOURCE_PATH("PCG_samples/pcg32_state123_seq456_first2048.csv")));

	std::vector<uint32> expectedValues;
	const auto& row = sampleFile.getRow(0);
	for(std::size_t i = 0; i < row.numValues(); ++i)
	{
		expectedValues.push_back(row.getValue<uint32>(i));
	}

	ASSERT_EQ(expectedValues.size(), 2048);

	uint32 seq = 456;
	uint32 state = 123;
	Pcg32 rng(seq, state);
	for(std::size_t i = 0; i < 2048; ++i)
	{
		EXPECT_EQ(rng.generate(), expectedValues[i]);
	}
}

TEST(Pcg32Test, JumpAhead)
{
	// Use a reference rng to generate expected values
	std::vector<uint32> expectedValues;
	{
		Pcg32 rng;
		for(std::size_t i = 0; i < 1024; ++i)
		{
			expectedValues.push_back(rng.generate());
		}
	}
	
	// Jump some distances and lookup to see if it is correct

	// 0-distance jump
	{
		Pcg32 rng;
		rng.jumpAhead(0);
		EXPECT_EQ(rng.generate(), expectedValues[0]);
	}
	
	// Single jump
	{
		for(std::size_t distance = 1; distance < expectedValues.size(); ++distance)
		{
			Pcg32 rng;
			rng.jumpAhead(distance);
			EXPECT_EQ(rng.generate(), expectedValues[distance]);
		}
	}

	// Multiple jumps
	{
		const uint64 distance1 = 12;
		const uint64 distance2 = 34;
		const uint64 distance3 = 56;

		Pcg32 rng;
		rng.jumpAhead(distance1);
		rng.jumpAhead(distance2);
		rng.jumpAhead(distance3);

		EXPECT_EQ(rng.generate(), expectedValues[distance1 + distance2 + distance3]);
	}
}
