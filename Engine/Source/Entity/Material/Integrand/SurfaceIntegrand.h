#pragma once

#include "Common/primitive_type.h"
#include "Math/Vector3f.h"
#include "Entity/Material/Integrand/SurfaceSample.h"

namespace ph
{

class Ray;
class Material;
class Intersection;

class SurfaceIntegrand
{
public:
	virtual ~SurfaceIntegrand() = 0;

	virtual void evaluateImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const = 0;
};

}// end namespace ph