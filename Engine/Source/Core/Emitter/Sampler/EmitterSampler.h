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
class DirectEnergySampleQuery;
class SurfaceHit;
class SampleFlow;

class EmitterSampler
{
public:
	virtual ~EmitterSampler();

	// FIXME: should update with emitters only
	virtual void update(const CookedDataStorage& cookedActors) = 0;
	virtual const Emitter* pickEmitter(SampleFlow& sampleFlow, real* const out_PDF) const = 0;
	virtual void genDirectSample(DirectEnergySampleQuery& query, SampleFlow& sampleFlow) const = 0;
	virtual real calcDirectPdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const = 0;
};

}// end namespace ph
