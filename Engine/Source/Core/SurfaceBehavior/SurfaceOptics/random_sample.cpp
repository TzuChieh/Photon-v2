#include "Core/SurfaceBehavior/SurfaceOptics/random_sample.h"
#include "Math/TVector3.h"
#include "Math/constant.h"

namespace ph
{

void genUnitHemisphereUniformSample(const real value_0_1_a, const real value_0_1_b, Vector3R* const out_sample)
{
	const real phi     = 2.0_r * PH_PI_REAL * value_0_1_a;
	const real yValue  = value_0_1_b;
	const real yRadius = sqrt(1.0_r - yValue * yValue);

	out_sample->x = cos(phi) * yRadius;
	out_sample->y = yValue;
	out_sample->z = sin(phi) * yRadius;
}

void genUnitHemisphereCosineThetaWeightedSample(const real value_0_1_a, const real value_0_1_b, Vector3R* const out_sample)
{
	const real phi     = 2.0f * PH_PI_REAL * value_0_1_a;
	const real yValue  = sqrt(value_0_1_b);
	const real yRadius = sqrt(1.0f - yValue * yValue);

	out_sample->x = cos(phi) * yRadius;
	out_sample->y = yValue;
	out_sample->z = sin(phi) * yRadius;
}

void genUnitHemisphereGgxTrowbridgeReitzNdfSample(const real value_0_1_a, const real value_0_1_b, const real alpha, Vector3R* const out_sample)
{
	// for GGX (Trowbridge-Reitz) Normal Distribution Function

	const real phi     = 2.0f * PH_PI_REAL * value_0_1_a;
	const real randNum = value_0_1_b;
	const real theta   = atan(alpha * sqrt(randNum / (1.0_r - randNum)));

	const real sinTheta = sin(theta);
	const real cosTheta = cos(theta);

	out_sample->x = cos(phi) * sinTheta;
	out_sample->y = cosTheta;
	out_sample->z = sin(phi) * sinTheta;
}

}// end namespace ph