#include "Math/random_number.h"

namespace ph
{

union union_bit32
{
	float32 float_value;
	uint32 uint_value;
};

static uint32 seedA = 17;
static uint32 seedB = 37;

// Marsaglia MWC generator
float32 genRandomFloat32_0_1_uniform()
{
	seedA = 36969 * ((seedA) & 65535) + ((seedA) >> 16);
	seedB = 18000 * ((seedB) & 65535) + ((seedB) >> 16);
	uint32 temp = ((seedA) << 16) + (seedB);

	union_bit32 result;
	result.uint_value = (temp & 0x007fffff) | 0x40000000;
	return (result.float_value - 2.0f) / 2.0f;
}

}// end namespace ph

