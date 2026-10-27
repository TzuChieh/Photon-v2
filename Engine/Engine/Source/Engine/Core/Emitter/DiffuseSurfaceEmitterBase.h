#pragma once

#include "Engine/Core/Emitter/SurfaceEmitter.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/Texture/TTexture.h"

namespace ph
{

class Primitive;

/*! @brief Base for diffusive surface emitters.
*/
class DiffuseSurfaceEmitterBase : public SurfaceEmitter
{
public:
	explicit DiffuseSurfaceEmitterBase(
		EmitterFeatureSet featureSet = defaultFeatureSet);

	void evalEmittedEnergy(const SurfaceHit& Xe, math::Spectrum* out_energy) const override = 0;

	void genDirectSample(DirectEnergySampleQuery& query, SampleFlow& sampleFlow) const override = 0;

	void calcDirectPdf(DirectEnergyPdfQuery& query) const override = 0;

	void emitRay(EnergyEmissionSampleQuery& query, SampleFlow& sampleFlow) const override = 0;

protected:
	void evalEnergyFromSurface(
		const TTexture<math::Spectrum>& energy,
		const SurfaceHit& Xe,
		math::Spectrum* out_energy) const;

	/*!
	@note
	- Generates a hit event in `DirectEnergySampleOutput::getHitEvent()`
	- Handles `EEmitterFeatureSet::DirectSample`
	*/
	void genDirectSampleFromSurface(
		const Primitive& surface,
		const TTexture<math::Spectrum>& energy,
		DirectEnergySampleQuery& query,
		SampleFlow& sampleFlow) const;

	/*!
	@note
	- Generates a hit event in `EnergyEmissionSampleOutput::getHitEvent()`
	- Handles `EEmitterFeatureSet::EmissionSample`
	*/
	void emitRayFromSurface(
		const Primitive& surface,
		const TTexture<math::Spectrum>& energy,
		EnergyEmissionSampleQuery& query,
		SampleFlow& sampleFlow) const;
};

}// end namespace ph
