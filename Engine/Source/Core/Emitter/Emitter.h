#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

namespace ph
{

class Primitive;
class Intersection;
class DirectLightSample;
class Ray;

class Emitter
{
public:
	Emitter();
	virtual ~Emitter() = 0;

	virtual void evalEmittedRadiance(const Intersection& intersection, Vector3R* const out_emitterRadiance) const = 0;
	virtual void genDirectSample(const Vector3R& targetPos, Vector3R* const out_emitPos, Vector3R* const out_emittedRadiance, real* const out_PDF) const = 0;
	virtual void genDirectSample(DirectLightSample& sample) const = 0;
	virtual void genSensingRay(Ray* const out_ray, Vector3R* const out_Le, Vector3R* const out_eN, real* const out_pdfA, real* const out_pdfW) const = 0;
	virtual real calcDirectSamplePdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Primitive* hitPrim) const = 0;

	virtual inline bool isSurfaceEmissive() const
	{
		return true;
	}
	
};

}// end namespace ph