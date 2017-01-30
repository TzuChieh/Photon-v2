#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <memory>
#include <vector>

namespace ph
{

class Emitter;
class Model;
class CookedActorStorage;
class Primitive;
class DirectLightSample;

class LightSampler
{
public:
	virtual ~LightSampler() = 0;

	virtual void update(const CookedActorStorage& cookedActors) = 0;
	virtual const Emitter* pickEmitter(float32* const out_PDF) const = 0;
	virtual void genDirectSample(DirectLightSample& sample) const = 0;
	virtual float32 calcDirectPdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Emitter* hitEmitter, const Primitive* hitPrim) const = 0;
};

}// end namespace ph