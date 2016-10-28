#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class Vector3f;
class Ray;

class SurfaceIntegrand
{
public:
	virtual ~SurfaceIntegrand() = 0;

	virtual void genUniformRandomLOverRegion(const Vector3f& N, Vector3f* out_L) const = 0;
	virtual bool sampleLiWeight(const Vector3f& L, const Vector3f& V, const Vector3f& N, Ray& ray) const = 0;
};

}// end namespace ph