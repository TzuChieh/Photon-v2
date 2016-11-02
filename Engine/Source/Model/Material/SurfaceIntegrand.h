#pragma once

#include "Common/primitive_type.h"
#include "Math/Vector3f.h"

namespace ph
{

class Ray;
class Material;
class Intersection;

class SurfaceIntegrand
{
public:
	virtual ~SurfaceIntegrand() = 0;

	virtual void genUniformRandomVOverRegion(const Vector3f& N, Vector3f* out_V) const = 0;
	virtual void sampleBRDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_BRDF) const = 0;

	virtual inline bool isEmissive() const
	{
		return false;
	}

	virtual inline void sampleEmittedRadiance(const Intersection& intersection, const Vector3f& L, const Vector3f& V, 
	                                          Vector3f* const out_emittedRadiance) const
	{
		out_emittedRadiance->set(0, 0, 0);
	}
};

}// end namespace ph