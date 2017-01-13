#pragma once

#include "Common/primitive_type.h"
#include "Math/Transform.h"

namespace ph
{

class Primitive;
class Vector3f;
class Intersection;
class DirectLightSample;
class Ray;

class Emitter
{
public:
	Transform worldToLocal;
	Transform localToWorld;

	Emitter();
	virtual ~Emitter() = 0;

	virtual void evalEmittedRadiance(const Intersection& intersection, Vector3f* const out_emitterRadiance) const = 0;
	virtual void genDirectSample(const Vector3f& targetPos, Vector3f* const out_emitPos, Vector3f* const out_emittedRadiance, float32* const out_PDF) const = 0;
	virtual void genDirectSample(DirectLightSample& sample) const = 0;
	virtual void genSensingRay(Ray* const out_ray, Vector3f* const out_Le, float32* const out_pdfA, float32* const out_pdfW) const = 0;
	virtual float32 calcDirectSamplePdfW(const Vector3f& targetPos, const Vector3f& emitPos, const Vector3f& emitN, const Primitive* hitPrim) const = 0;

	virtual inline bool isSurfaceEmissive() const
	{
		return true;
	}
	
};

}// end namespace ph