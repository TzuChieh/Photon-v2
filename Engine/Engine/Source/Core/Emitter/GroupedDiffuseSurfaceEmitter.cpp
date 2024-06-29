#include "Core/Emitter/GroupedDiffuseSurfaceEmitter.h"
#include "Core/Emitter/DiffuseSurfaceEmitter.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Texture/TSampler.h"
#include "Math/Random/Random.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/Emitter/Query/DirectEnergyPdfQuery.h"
#include "Core/Emitter/Query/EnergyEmissionSampleQuery.h"

#include <Common/utility.h>
#include <Common/logging.h>

#include <unordered_set>

namespace ph
{

GroupedDiffuseSurfaceEmitter::GroupedDiffuseSurfaceEmitter(
	TSpanView<const Primitive*> surfaces,
	const std::shared_ptr<TTexture<math::Spectrum>>& emittedEnergy,
	const EmitterFeatureSet featureSet)
	
	: DiffuseSurfaceEmitterBase(featureSet)
	
	, m_surfaces(surfaces.begin(), surfaces.end())
	, m_emittedEnergy(emittedEnergy)
{
#if PH_DEBUG
	// Performance warning
	if(m_surfaces.size() <= 1)
	{
		PH_DEFAULT_LOG(Warning,
			"`GroupedDiffuseSurfaceEmitter` expects at least 2 primitives surfaces bundled together "
			"to gain better efficiency, {} surface is given.", m_surfaces.size());
	}

	// Check for duplicated surfaces
	std::unordered_set<const Primitive*> surfaceSet;
	for(const Primitive* surface : surfaces)
	{
		if(surfaceSet.contains(surface))
		{
			PH_DEFAULT_LOG(Warning,
				"Adding duplicated surface primitive {} to `GroupedDiffuseSurfaceEmitter`.",
				static_cast<const void*>(surface));
		}

		surfaceSet.insert(surface);
	}
#endif
}

void GroupedDiffuseSurfaceEmitter::evalEmittedEnergy(const SurfaceHit& Xe, math::Spectrum* const out_energy) const
{
	evalEnergyFromSurface(getEmittedEnergy(), Xe, out_energy);
}

void GroupedDiffuseSurfaceEmitter::genDirectSample(
	DirectEnergySampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	if(getFeatureSet().hasNo(EEmitterFeatureSet::DirectSample))
	{
		return;
	}

	// FIXME: use sampleFlow
	const Primitive& surface = getSurface(math::Random::index(0, numSurfaces()));
	genDirectSampleFromSurface(surface, getEmittedEnergy(), query, sampleFlow, probe);
	if(!query.outputs)
	{
		return;
	}

	const real pickPdf = 1.0_r / numSurfaces();
	query.outputs.setPdf(query.outputs.getPdf() * pickPdf);
}

void GroupedDiffuseSurfaceEmitter::calcDirectPdf(DirectEnergyPdfQuery& query) const
{
	const real pickPdf = 1.0_r / numSurfaces();
	calcDirectPdfWForSrcPrimitive(query, lta::PDF::D(pickPdf));
}

void GroupedDiffuseSurfaceEmitter::emitRay(
	EnergyEmissionSampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	if(getFeatureSet().hasNo(EEmitterFeatureSet::EmissionSample))
	{
		return;
	}

	// Randomly and uniformly pick a primitive

	// FIXME: use sampleFlow
	const Primitive& surface = getSurface(math::Random::index(0, numSurfaces()));
	emitRayFromSurface(surface, getEmittedEnergy(), query, sampleFlow, probe);
	if(!query.outputs)
	{
		return;
	}

	const real pickPdf = 1.0_r / numSurfaces();
	query.outputs.setPdf(query.outputs.getPdfPos() * pickPdf, query.outputs.getPdfDir());
}

real GroupedDiffuseSurfaceEmitter::calcRadiantFluxApprox() const
{
	real totalRadiantFlux = 0.0_r;
	for(const Primitive* surface : m_surfaces)
	{
		DiffuseSurfaceEmitter surfaceEmitter(surface, m_emittedEnergy);
		totalRadiantFlux += surfaceEmitter.calcRadiantFluxApprox();
	}

	return totalRadiantFlux > 0.0_r ? totalRadiantFlux : SurfaceEmitter::calcRadiantFluxApprox();
}

}// end namespace ph
