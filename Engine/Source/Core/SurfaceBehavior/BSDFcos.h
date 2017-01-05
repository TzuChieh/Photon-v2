#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class Intersection;
class Ray;
class SurfaceSample;
class Vector3f;

class BSDFcos
{
public:
	virtual ~BSDFcos() = 0;

	virtual void genImportanceSample(SurfaceSample& sample) const = 0;
	virtual float32 calcImportanceSamplePdfW(const SurfaceSample& sample) const = 0;
	virtual void evaluate(SurfaceSample& sample) const = 0;
};

}// end namespace ph