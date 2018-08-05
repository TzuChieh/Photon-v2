#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <memory>
#include <vector>

namespace ph
{

class Emitter;
class Model;
class CookedDataStorage;
class Primitive;
class DirectLightSample;
class SurfaceHit;

class LightSampler
{
public:
	virtual ~LightSampler() = 0;

	// FIXME: should update with emitters only
	virtual void update(const CookedDataStorage& cookedActors) = 0;
	virtual const Emitter* pickEmitter(real* const out_PDF) const = 0;
	virtual void genDirectSample(DirectLightSample& sample) const = 0;
	virtual real calcDirectPdfW(const SurfaceHit& emitPos, const Vector3R& targetPos) const = 0;
};

}// end namespace ph