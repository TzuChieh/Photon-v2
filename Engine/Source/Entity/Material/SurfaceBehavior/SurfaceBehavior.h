#pragma once

#include "Common/primitive_type.h"
#include "Math/Vector3f.h"
#include "Entity/Material/SurfaceBehavior/SurfaceSample.h"

namespace ph
{

class Ray;
class Material;
class Intersection;

class SurfaceBehavior
{
public:
	virtual ~SurfaceBehavior() = 0;

	virtual void genBsdfCosImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const = 0;
	//virtual void evaluate(const Intersection& intersection, const Vector3f& wi, const Vector3f& wo);
};

}// end namespace ph