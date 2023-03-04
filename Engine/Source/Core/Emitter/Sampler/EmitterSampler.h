#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <memory>
#include <vector>
#include <span>

namespace ph
{

class Emitter;
class Model;
class Primitive;
class DirectEnergySampleQuery;
class SurfaceHit;
class SampleFlow;

class EmitterSampler
{
public:
	virtual ~EmitterSampler();

	virtual void update(std::span<const Emitter*> emitters) = 0;
	virtual const Emitter* pickEmitter(SampleFlow& sampleFlow, real* const out_PDF) const = 0;
	virtual void genDirectSample(DirectEnergySampleQuery& query, SampleFlow& sampleFlow) const = 0;
	virtual real calcDirectPdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const = 0;
};

}// end namespace ph
