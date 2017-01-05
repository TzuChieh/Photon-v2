#pragma once

#include "Common/primitive_type.h"

#include <memory>
#include <vector>

namespace ph
{

class Emitter;
class Model;
class CookedActorStorage;
class Vector3f;
class Primitive;
class DirectLightSample;

class LightSampler
{
public:
	virtual ~LightSampler() = 0;

	virtual void update(const CookedActorStorage& cookedActors) = 0;
	virtual const Emitter* pickEmitter(float32* const out_PDF) const = 0;
	virtual void genDirectSample(DirectLightSample& sample) const = 0;
	virtual float32 calcDirectPdfW(const Vector3f& targetPos, const Vector3f& emitPos, const Vector3f& emitN, const Emitter* hitEmitter, const Primitive* hitPrim) const = 0;
};

}// end namespace ph