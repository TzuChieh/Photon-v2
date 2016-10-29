#pragma once

#include "Model/Material/LightSurfaceIntegrand.h"
#include "Math/Vector3f.h"
#include "Core/Ray.h"
#include "Math/random_number.h"
#include "Math/constant.h"

namespace ph
{

LightSurfaceIntegrand::LightSurfaceIntegrand() : 
	m_emittedRadiance(0, 0, 0)
{

}

LightSurfaceIntegrand::~LightSurfaceIntegrand() = default;

void LightSurfaceIntegrand::genUniformRandomLOverRegion(const Vector3f& N, Vector3f* out_L) const
{
	const float32 rand1 = genRandomFloat32_0_1_uniform();
	const float32 rand2 = genRandomFloat32_0_1_uniform();

	const float32 phi = 2.0f * PI_FLOAT32 * rand1;
	const float32 yValue = rand2;
	const float32 yRadius = sqrt(1.0f - yValue * yValue);

	out_L->x = cos(phi) * yRadius;
	out_L->y = yValue;
	out_L->z = sin(phi) * yRadius;

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

void LightSurfaceIntegrand::sampleBRDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_BRDF) const
{

}

}// end namespace ph