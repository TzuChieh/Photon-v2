#pragma once

#include "Model/Material/LightSurfaceIntegrand.h"
#include "Math/Vector3f.h"
#include "Core/Ray.h"

namespace ph
{

LightSurfaceIntegrand::~LightSurfaceIntegrand() = default;

bool LightSurfaceIntegrand::sampleLiWeight(const Vector3f& L, const Vector3f& V, const Vector3f& N, Ray& ray) const
{
	ray.addLiRadiance(Vector3f(1.0f));

	return false;
}

}// end namespace ph