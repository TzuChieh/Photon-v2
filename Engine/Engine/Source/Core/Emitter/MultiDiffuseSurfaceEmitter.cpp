#include "Core/Emitter/MultiDiffuseSurfaceEmitter.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Texture/TSampler.h"
#include "Math/Random/Random.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/Emitter/Query/DirectEnergyPdfQuery.h"
#include "Core/Emitter/Query/EnergyEmissionSampleQuery.h"

#include <Common/utility.h>
#include <Common/logging.h>

namespace ph
{

MultiDiffuseSurfaceEmitter::MultiDiffuseSurfaceEmitter(
	TSpanView<DiffuseSurfaceEmitter> emitters,
	const EmitterFeatureSet featureSet)
	
	: SurfaceEmitter(featureSet)
	
	, m_emitters()
	, m_primitiveToEmitterIdx()
{
#if PH_DEBUG
	if(emitters.size() <= 1)
	{
		PH_DEFAULT_LOG(Warning,
			"`MultiDiffuseSurfaceEmitter` expects at least 2 primitives surfaces bundled together "
			"to gain better efficiency, {} surface is given.", emitters.size());
	}
#endif

	for(const DiffuseSurfaceEmitter& emitter : emitters)
	{
		addEmitter(emitter);
	}
}

void MultiDiffuseSurfaceEmitter::evalEmittedEnergy(const SurfaceHit& X, math::Spectrum* const out_energy) const
{
	const Primitive* surface = X.getDetail().getPrimitive();
	const auto findResult = m_primitiveToEmitterIdx.find(surface);
	PH_ASSERT(findResult != m_primitiveToEmitterIdx.end());

	const auto emitterIdx = findResult->second;
	const DiffuseSurfaceEmitter& emitter = m_emitters[emitterIdx];

	// We cannot just use the first emitter since each emitter may use a different energy map
	emitter.evalEmittedEnergy(X, out_energy);
}

void MultiDiffuseSurfaceEmitter::genDirectSample(
	DirectEnergySampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	if(getFeatureSet().hasNo(EEmitterFeatureSet::DirectSample))
	{
		return;
	}

	// FIXME: use sampleFlow
	PH_ASSERT(!m_emitters.empty());
	const DiffuseSurfaceEmitter& emitter = m_emitters[math::Random::index(0, m_emitters.size())];

	emitter.genDirectSample(query, sampleFlow, probe);
	if(!query.outputs)
	{
		return;
	}

	const real pickPdf = (1.0_r / static_cast<real>(m_emitters.size()));
	query.outputs.setPdf(query.outputs.getPdf() * pickPdf);
}

void MultiDiffuseSurfaceEmitter::calcDirectPdf(DirectEnergyPdfQuery& query) const
{
	PH_ASSERT(!m_emitters.empty());
	const real pickPdf = (1.0_r / static_cast<real>(m_emitters.size()));
	calcDirectPdfWForSrcPrimitive(query, lta::PDF::D(pickPdf));
}

void MultiDiffuseSurfaceEmitter::emitRay(
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
	const DiffuseSurfaceEmitter& emitter = m_emitters[math::Random::index(0, m_emitters.size())];
	emitter.emitRay(query, sampleFlow, probe);
	if(!query.outputs)
	{
		return;
	}

	const real pickPdf = 1.0_r / static_cast<real>(m_emitters.size());
	query.outputs.setPdf(query.outputs.getPdfPos() * pickPdf, query.outputs.getPdfDir());
}

DiffuseSurfaceEmitter& MultiDiffuseSurfaceEmitter::addEmitter(
	const DiffuseSurfaceEmitter& emitter)
{
	m_emitters.push_back(emitter);

	const Primitive& surface = m_emitters.back().getSurface();
#if PH_DEBUG
	if(m_primitiveToEmitterIdx.contains(&surface))
	{
		PH_DEFAULT_LOG(Warning,
			"Adding duplicated surface primitive to `MultiDiffuseSurfaceEmitter`.");
	}
#endif
	m_primitiveToEmitterIdx[&surface] = m_emitters.size() - 1;

	if(m_isBackFaceEmission)
	{
		m_emitters.back().setBackFaceEmit();
	}
	else
	{
		m_emitters.back().setFrontFaceEmit();
	}

	return m_emitters.back();
}

void MultiDiffuseSurfaceEmitter::setFrontFaceEmit()
{
	SurfaceEmitter::setFrontFaceEmit();

	for(auto& emitter : m_emitters)
	{
		emitter.setFrontFaceEmit();
	}
}

void MultiDiffuseSurfaceEmitter::setBackFaceEmit()
{
	SurfaceEmitter::setBackFaceEmit();

	for(DiffuseSurfaceEmitter& emitter : m_emitters)
	{
		emitter.setBackFaceEmit();
	}
}

real MultiDiffuseSurfaceEmitter::calcRadiantFluxApprox() const
{
	real totalRadiantFlux = 0.0_r;
	for(const DiffuseSurfaceEmitter& emitter : m_emitters)
	{
		totalRadiantFlux += emitter.calcRadiantFluxApprox();
	}

	return totalRadiantFlux > 0.0_r ? totalRadiantFlux : SurfaceEmitter::calcRadiantFluxApprox();
}

}// end namespace ph
