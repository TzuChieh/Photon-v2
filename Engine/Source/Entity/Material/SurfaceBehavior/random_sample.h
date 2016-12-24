#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class Vector3f;

// sample PDF is 1/(2*pi)
extern void genUnitHemisphereUniformSample(const float32 value_0_1_a, const float32 value_0_1_b, Vector3f* const out_sample);

// sample PDF is cos(theta)
extern void genUnitHemisphereCosineThetaWeightedSample(const float32 value_0_1_a, const float32 value_0_1_b, Vector3f* const out_sample);

// sample PDF is NDF*cos(NoH)
extern void genUnitHemisphereGgxTrowbridgeReitzNdfSample(const float32 value_0_1_a, const float32 value_0_1_b, const float32 roughness, Vector3f* const out_sample);

}// end namespace ph