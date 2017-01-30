#include "Core/SurfaceBehavior/random_sample.h"
#include "Math/TVector3.h"
#include "Math/constant.h"

namespace ph
{

void genUnitHemisphereUniformSample(const float32 value_0_1_a, const float32 value_0_1_b, Vector3R* const out_sample)
{
	const float32 phi     = 2.0f * PI_FLOAT32 * value_0_1_a;
	const float32 yValue  = value_0_1_b;
	const float32 yRadius = sqrt(1.0f - yValue * yValue);

	out_sample->x = cos(phi) * yRadius;
	out_sample->y = yValue;
	out_sample->z = sin(phi) * yRadius;
}

void genUnitHemisphereCosineThetaWeightedSample(const float32 value_0_1_a, const float32 value_0_1_b, Vector3R* const out_sample)
{
	const float32 phi     = 2.0f * PI_FLOAT32 * value_0_1_a;
	const float32 yValue  = sqrt(value_0_1_b);
	const float32 yRadius = sqrt(1.0f - yValue * yValue);

	out_sample->x = cos(phi) * yRadius;
	out_sample->y = yValue;
	out_sample->z = sin(phi) * yRadius;
}

void genUnitHemisphereGgxTrowbridgeReitzNdfSample(const float32 value_0_1_a, const float32 value_0_1_b, const float32 alpha, Vector3R* const out_sample)
{
	// for GGX (Trowbridge-Reitz) Normal Distribution Function

	const float32 phi     = 2.0f * PI_FLOAT32 * value_0_1_a;
	const float32 randNum = value_0_1_b;
	const float32 theta   = atan(alpha * sqrt(randNum / (1.0f - randNum)));

	const float32 sinTheta = sin(theta);
	const float32 cosTheta = cos(theta);

	out_sample->x = cos(phi) * sinTheta;
	out_sample->y = cosTheta;
	out_sample->z = sin(phi) * sinTheta;
}

}// end namespace ph