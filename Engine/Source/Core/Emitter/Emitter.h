#pragma once

#include "Common/primitive_type.h"
#include "Math/Transform.h"

namespace ph
{

class Vector3f;
class Intersection;

class Emitter
{
public:
	Transform worldToLocal;
	Transform localToWorld;

	Emitter();
	virtual ~Emitter() = 0;

	virtual void evalEmittedRadiance(const Intersection& intersection, Vector3f* const out_emitterRadiance) const = 0;
	virtual void samplePosition(const Vector3f& position, const Vector3f& direction, float32* const out_PDF, Vector3f* const out_emittedRadiance) const = 0;

	virtual inline bool isSurfaceEmissive() const
	{
		return true;
	}
	
};

}// end namespace ph