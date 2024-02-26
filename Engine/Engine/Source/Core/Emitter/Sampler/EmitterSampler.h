#pragma once

#include "Math/math_fwd.h"
#include "Utility/TSpan.h"

#include <Common/primitive_type.h>

#include <memory>
#include <vector>

namespace ph
{

class Emitter;
class DirectEnergySampleQuery;
class DirectEnergySamplePdfQuery;
class HitProbe;
class SurfaceHit;
class SampleFlow;

class EmitterSampler
{
public:
	virtual ~EmitterSampler();

	virtual void update(TSpanView<const Emitter*> emitters) = 0;
	virtual const Emitter* pickEmitter(SampleFlow& sampleFlow, real* const out_PDF) const = 0;

	/*! @brief Sample direct lighting for a target position.

	@note Generates hit event (with `DirectEnergySampleOutput::getObservationRay()` and `probe`).
	*/
	virtual void genDirectSample(
		DirectEnergySampleQuery& query, 
		SampleFlow& sampleFlow,
		HitProbe& probe) const = 0;

	/*!
	@note Generates hit event (with `DirectEnergySamplePdfInput::getObservationRay()` and `probe`).
	*/
	virtual void calcDirectSamplePdfW(
		DirectEnergySamplePdfQuery& query,
		HitProbe& probe) const = 0;
};

}// end namespace ph
