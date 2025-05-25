#pragma once

#include "Engine/Core/Emitter/Emitter.h"

namespace ph
{

class VolumeEmitter : public Emitter
{
public:
	using Emitter::Emitter;

	void evalEmittedEnergy(const SurfaceHit& Xe, math::Spectrum* out_energy) const override = 0;

	void genDirectSample(
		DirectEnergySampleQuery& query, 
		SampleFlow& sampleFlow,
		HitProbe& probe) const override = 0;

	void calcDirectPdf(DirectEnergyPdfQuery& query) const override = 0;

	void emitRay(
		EnergyEmissionSampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override = 0;
};

}// end namespace ph
