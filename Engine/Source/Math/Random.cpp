#include "Math/Random.h"
#include "Math/math.h"
#include "Common/assertion.h"
#include "Math/Random/DeterministicSeeder.h"

#include <random>

namespace ph::math
{

std::atomic<int32> Random::seed(37);
const int32 Random::incrementation = 17;

// NOTE: consider using __rdtsc() as seed (if film merging is desired)
real Random::genUniformReal_i0_e1()
{
	// TODO: check whether std::mt19937 can be used for 64-bit random number generating
	//static thread_local std::mt19937 generator(seed += incrementation);
	static thread_local std::mt19937 generator(DeterministicSeeder::nextSeed<uint32>());

	// NOTE: we can just create a new distribution for each call
	// (profile the code to find out which is better)
	static thread_local std::uniform_real_distribution<real> distribution(0.0_r, 1.0_r);

	return distribution(generator);
}

std::size_t Random::genUniformIndex_iL_eU(const std::size_t lowerBound,
                                          const std::size_t upperBound)
{
	PH_ASSERT(upperBound > lowerBound);

	const std::size_t numIntervals = upperBound - lowerBound;
	std::size_t index = static_cast<std::size_t>(lowerBound + genUniformReal_i0_e1() * numIntervals);

	return index < lowerBound ? lowerBound : (index >= upperBound ? upperBound - 1 : index);
}

// FIXME: type-punning with unions is undefined behavior
//union union_bit32
//{
//	float32 float_value;
//	uint32 uint_value;
//};
//
//static uint32 seedA = 17;
//static uint32 seedB = 37;
//
//// Marsaglia MWC generator
//float32 genRandomFloat32_0_1_uniform()
//{
//	seedA = 36969 * ((seedA) & 65535) + ((seedA) >> 16);
//	seedB = 18000 * ((seedB) & 65535) + ((seedB) >> 16);
//	uint32 temp = ((seedA) << 16) + (seedB);
//
//	union_bit32 result;
//	result.uint_value = (temp & 0x007fffff) | 0x40000000;
//	return (result.float_value - 2.0f) / 2.0f;
//}

}// end namespace ph::math

