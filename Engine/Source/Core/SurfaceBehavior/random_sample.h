#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

namespace ph
{

// sample PDF is 1/(2*pi)
extern void genUnitHemisphereUniformSample(const real value_0_1_a, const real value_0_1_b, Vector3R* const out_sample);

// sample PDF is cos(theta)
extern void genUnitHemisphereCosineThetaWeightedSample(const real value_0_1_a, const real value_0_1_b, Vector3R* const out_sample);

// sample PDF is NDF*cos(NoH)
extern void genUnitHemisphereGgxTrowbridgeReitzNdfSample(const real value_0_1_a, const real value_0_1_b, const real alpha, Vector3R* const out_sample);

}// end namespace ph