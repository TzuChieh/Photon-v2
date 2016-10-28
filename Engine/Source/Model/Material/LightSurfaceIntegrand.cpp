#pragma once

#include "Model/Material/LightSurfaceIntegrand.h"
#include "Math/Vector3f.h"
#include "Core/Ray.h"
#include "Math/random_number.h"
#include "Math/constant.h"

namespace ph
{

LightSurfaceIntegrand::~LightSurfaceIntegrand() = default;

void LightSurfaceIntegrand::genUniformRandomLOverRegion(const Vector3f& N, Vector3f* out_L) const
{
	const float32 rand1 = genRandomFloat32_0_1_uniform();
	const float32 rand2 = genRandomFloat32_0_1_uniform();

	out_L->x = 2.0f * cos(2.0 * PI_FLOAT32 * rand1) * sqrt(rand2 * (1.0f - rand2));
	out_L->y = 1.0f - 2.0f * rand2;
	out_L->z = 2.0f * sin(2.0 * PI_FLOAT32 * rand1) * sqrt(rand2 * (1.0f - rand2));

	Vector3f u;
	Vector3f v(N);
	Vector3f w;

	v.calcOrthBasisAsYaxis(&u, &w);

	*out_L = u.mulLocal(out_L->x).
	addLocal(v.mulLocal(out_L->y)).
	addLocal(w.mulLocal(out_L->z));

	out_L->normalizeLocal();
}

bool LightSurfaceIntegrand::sampleLiWeight(const Vector3f& L, const Vector3f& V, const Vector3f& N, Ray& ray) const
{
	ray.addLiRadiance(Vector3f(1.0f));

	return false;
}

}// end namespace ph