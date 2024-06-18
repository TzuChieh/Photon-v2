#include "Core/Emitter/MultiDiffuseSurfaceEmitter.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Texture/TSampler.h"
#include "Math/Color/Spectrum.h"
#include "Core/Texture/TTexture.h"
#include "Math/Random/Random.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/Emitter/Query/DirectEnergyPdfQuery.h"
#include "Core/Emitter/Query/EnergyEmissionSampleQuery.h"

#include <Common/assertion.h>
#include <Common/utility.h>

namespace ph
{

MultiDiffuseSurfaceEmitter::MultiDiffuseSurfaceEmitter(const std::vector<DiffuseSurfaceEmitter>& emitters) :
	SurfaceEmitter(),
	m_emitters()
{
	PH_ASSERT(!emitters.empty());

	m_extendedArea = 0.0_r;
	for(const auto& emitter : emitters)
	{
		const Primitive* primitive = emitter.getSurface();

		PH_ASSERT(primitive != nullptr);
		m_extendedArea += primitive->calcExtendedArea();

		addEmitter(emitter);
	}
	m_reciExtendedArea = 1.0_r / m_extendedArea;
}

void MultiDiffuseSurfaceEmitter::evalEmittedRadiance(const SurfaceHit& X, math::Spectrum* const out_radiance) const
{
	PH_ASSERT(!m_emitters.empty());

	m_emitters.front().evalEmittedRadiance(X, out_radiance);
}

void MultiDiffuseSurfaceEmitter::genDirectSample(
	DirectEnergySampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	PH_ASSERT(!m_emitters.empty());

	// FIXME: use sampleFlow
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
	// Randomly and uniformly pick a primitive

	// FIXME: use sampleFlow
	const auto& emitter = m_emitters[math::Random::index(0, m_emitters.size())];
	const real pickPdf = 1.0_r / static_cast<real>(m_emitters.size());

	emitter.emitRay(query, sampleFlow, probe);
	if(!query.outputs)
	{
		return;
	}

	query.outputs.setPdf(query.outputs.getPdfPos() * pickPdf, query.outputs.getPdfDir());
}

void MultiDiffuseSurfaceEmitter::setEmittedRadiance(
	const std::shared_ptr<TTexture<math::Spectrum>>& emittedRadiance)
{
	for(auto& emitter : m_emitters)
	{
		emitter.setEmittedRadiance(emittedRadiance);
	}
}

void MultiDiffuseSurfaceEmitter::addEmitter(const DiffuseSurfaceEmitter& emitter)
{
	m_emitters.push_back(emitter);

	if(m_isBackFaceEmission)
	{
		m_emitters.back().setBackFaceEmit();
	}
	else
	{
		m_emitters.back().setFrontFaceEmit();
	}
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

	for(auto& emitter : m_emitters)
	{
		emitter.setBackFaceEmit();
	}
}

const TTexture<math::Spectrum>& MultiDiffuseSurfaceEmitter::getEmittedRadiance() const
{
	PH_ASSERT(!m_emitters.empty());

	return m_emitters.front().getEmittedRadiance();
}

real MultiDiffuseSurfaceEmitter::calcRadiantFluxApprox() const
{
	real totalRadiantFlux = 0.0_r;
	for(const auto& emitter : m_emitters)
	{
		totalRadiantFlux += emitter.calcRadiantFluxApprox();
	}

	return totalRadiantFlux > 0.0_r ? totalRadiantFlux : SurfaceEmitter::calcRadiantFluxApprox();
}

}// end namespace ph
