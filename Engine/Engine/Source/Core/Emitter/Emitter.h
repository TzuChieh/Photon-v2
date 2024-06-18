#pragma once

#include "Math/math_fwd.h"
#include "Math/Color/Spectrum.h"

#include <Common/primitive_type.h>

namespace ph
{

class SurfaceHit;
class HitProbe;
class DirectEnergySampleQuery;
class DirectEnergyPdfQuery;
class EnergyEmissionSampleQuery;
class SampleFlow;

/*! @brief An electromagnetic energy emitting source.
The emitted energy can be captured by a `Receiver`.
*/
class Emitter
{
public:
	Emitter();
	virtual ~Emitter();

	virtual void evalEmittedRadiance(const SurfaceHit& X, math::Spectrum* out_radiance) const = 0;

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

	/*! @brief Emit a ray that carries some amount of energy from this emitter.
	@note Generates hit event (with `EnergyEmissionSampleOutput::getEmittedRay()` and `probe`).
	*/
	virtual void emitRay(
		EnergyEmissionSampleQuery& query, 
		SampleFlow& sampleFlow,
		HitProbe& probe) const = 0;

	virtual real calcRadiantFluxApprox() const;
};

// In-header Implementations:

inline real Emitter::calcRadiantFluxApprox() const
{
	// Non-zero to avoid not being sampled
	return 1.0_r;
}

}// end namespace ph
