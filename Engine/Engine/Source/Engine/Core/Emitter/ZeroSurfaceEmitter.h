#pragma once

#include "Engine/Core/Emitter/SurfaceEmitter.h"

namespace ph
{

class ZeroSurfaceEmitter : public SurfaceEmitter
{
public:
	ZeroSurfaceEmitter();

	void evalEmittedEnergy(const SurfaceHit& Xe, math::Spectrum* out_energy) const override;

	void genDirectSample(
		DirectEnergySampleQuery& query, 
		SampleFlow& sampleFlow,
		HitProbe& probe) const override;

	void calcDirectPdf(DirectEnergyPdfQuery& query) const override;

	void emitRay(
		EnergyEmissionSampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override;

	real calcRadiantFluxApprox() const override;
};

}// end namespace ph
