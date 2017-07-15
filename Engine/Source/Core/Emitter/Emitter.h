#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Math/Transform/Transform.h"

#include <memory>

namespace ph
{

class Primitive;
class Intersection;
class DirectLightSample;
class Ray;
class Time;

// TODO: supports motion blur on Emitters

class Emitter
{
public:
	Emitter();
	virtual ~Emitter() = 0;

	virtual void evalEmittedRadiance(const Intersection& intersection, Vector3R* out_emitterRadiance) const = 0;
	virtual void genDirectSample(const Vector3R& targetPos, Vector3R* out_emitPos, Vector3R* out_emittedRadiance, real* out_PDF) const = 0;
	virtual void genDirectSample(DirectLightSample& sample) const = 0;

	// FIXME: ray time
	virtual void genSensingRay(Ray* out_ray, Vector3R* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const = 0;

	virtual real calcDirectSamplePdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Primitive* hitPrim) const = 0;

	virtual inline bool isSurfaceEmissive() const
	{
		return true;
	}

	void setTransform(const Transform* localToWorld,
	                  const Transform* worldToLocal);
	
protected:
	const Transform* m_localToWorld;
	const Transform* m_worldToLocal;
};

}// end namespace ph