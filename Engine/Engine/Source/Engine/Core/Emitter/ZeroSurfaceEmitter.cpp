#include "Engine/Core/Emitter/ZeroSurfaceEmitter.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Engine/Core/Emitter/Query/DirectEnergyPdfQuery.h"
#include "Engine/Core/Emitter/Query/EnergyEmissionSampleQuery.h"

#include <Common/assertion.h>

namespace ph
{

ZeroSurfaceEmitter::ZeroSurfaceEmitter()
	: SurfaceEmitter()
{}

void ZeroSurfaceEmitter::evalEmittedEnergy(const SurfaceHit& Xe, math::Spectrum* out_energy) const
{
	PH_ASSERT(out_energy);
	out_energy->setColorValues(0);
}

void ZeroSurfaceEmitter::genDirectSample(
	DirectEnergySampleQuery& query,
	SampleFlow& sampleFlow) const
{
	query.outputs.setPdf({});
}

void ZeroSurfaceEmitter::calcDirectPdf(DirectEnergyPdfQuery& query) const
{
	query.outputs.setPdf({});
}

void ZeroSurfaceEmitter::emitRay(EnergyEmissionSampleQuery& query, SampleFlow& sampleFlow) const
{
	query.outputs.setPdf({}, {});
}

real ZeroSurfaceEmitter::calcRadiantFluxApprox() const
{
	return 0;
}

}// end namespace ph
