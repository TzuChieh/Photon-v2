#include "Math/Random.h"

namespace ph
{

const std::uniform_real_distribution<real> Random::distribution(0.0_r, 1.0_r);
std::atomic<int32> Random::seed(37);
const int32 Random::incrementation = 17;

// NOTE: consider using __rdtsc() as seed (if film merging is desired)
real Random::genUniformReal_i0_e1()
{
	// TODO: check whether std::mt19937 can be used for 64-bit random number generating
	static thread_local std::mt19937 generator(seed += incrementation);
	return distribution(generator);
}

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

}// end namespace ph

