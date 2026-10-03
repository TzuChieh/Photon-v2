#pragma once

#include "Engine/Math/math_fwd.h"
#include "Engine/Utility/TSpan.h"

#include <Common/primitive_type.h>

#include <memory>
#include <vector>

namespace ph
{

class Emitter;
class DirectEnergySampleQuery;
class DirectEnergyPdfQuery;
class HitProbe;
class SurfaceHit;
class SampleFlow;

class EmitterSampler
{
public:
	virtual ~EmitterSampler();

	/*! @brief Build one sampling population from disjoint emitter lists.
	@param nonPhysicalEmitters Nonblocking or directly invisible. Both lists share the same
	selection probabilities for direct sampling, emission sampling, and PDF evaluation.
	*/
	virtual void update(
		TSpanView<const Emitter*> emitters,
		TSpanView<const Emitter*> nonPhysicalEmitters) = 0;

	virtual const Emitter* pickEmitter(SampleFlow& sampleFlow, real* out_pdf) const = 0;

	/*! @brief Sample direct lighting for a target position.
	@note Generates hit event (with `DirectEnergySampleOutput::getObservationRay()` and `probe`).
	*/
	virtual void genDirectSample(
		DirectEnergySampleQuery& query, 
		SampleFlow& sampleFlow,
		HitProbe& probe) const = 0;

	/*! @brief Calculate the PDF of direct lighting for a target position.
	*/
	virtual void calcDirectPdf(DirectEnergyPdfQuery& query) const = 0;
};

}// end namespace ph
