#pragma once

#include "Common/primitive_type.h"
#include "Math/Vector3f.h"
#include "Model/Material/Integrand/SurfaceSample.h"

namespace ph
{

class Ray;
class Material;
class Intersection;

class SurfaceIntegrand
{
public:
	virtual ~SurfaceIntegrand() = 0;

	virtual void genUniformRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const = 0;
	virtual void genImportanceRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const = 0;
	virtual void evaluateUniformRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const = 0;
	virtual void evaluateImportanceRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const = 0;
	virtual void evaluateLiWeight(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_LiWeight) const = 0;

	virtual void evaluateImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const = 0;

	virtual inline bool isEmissive() const
	{
		return false;
	}

	virtual inline void evaluateEmittedRadiance(const Intersection& intersection, const Vector3f& L, const Vector3f& V, 
	                                            Vector3f* const out_emittedRadiance) const
	{
		out_emittedRadiance->set(0, 0, 0);
	}
};

}// end namespace ph