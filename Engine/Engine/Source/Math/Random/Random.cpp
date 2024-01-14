#include "Math/Random/Random.h"
#include "Math/math.h"
#include "Math/Random/DeterministicSeeder.h"
#include "Math/Random/TMt19937.h"
#include "Math/Random/Pcg32.h"
#include "Math/Random/Pcg64DXSM.h"
#include "Math/Random/TUrbg32x2.h"

#include <Common/assertion.h>

#include <random>

namespace ph::math
{

// NOTE: consider using __rdtsc() as seed (if film merging is desired)
real Random::sample()
{
	//static thread_local std::mt19937 generator(DeterministicSeeder::nextSeed<uint32>());
	//static thread_local TMt19937<uint32> generator(DeterministicSeeder::nextSeed<uint32>());
	static thread_local Pcg32 generator(DeterministicSeeder::nextSeed<uint32>());
	/*static thread_local Pcg64DXSM generator(
		DeterministicSeeder::nextSeed<uint64>(),
		DeterministicSeeder::nextSeed<uint64>());*/
	/*static thread_local TUrbg32x2<Pcg32, Pcg32> generator(
		Pcg32{DeterministicSeeder::nextSeed<uint32>()},
		Pcg32{DeterministicSeeder::nextSeed<uint32>()});*/

	// NOTE: we can just create a new distribution for each call
	// (profile the code to find out which is better)
	//static thread_local std::uniform_real_distribution<real> distribution(0.0_r, 1.0_r);

	//return distribution(generator);
	return generator.generateSample();
	//return static_cast<real>(generator.generateSample<float64>());
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

