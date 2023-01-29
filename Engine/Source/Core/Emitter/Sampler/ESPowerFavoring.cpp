#include "Core/Emitter/Sampler/ESPowerFavoring.h"
#include "World/Foundation/CookedDataStorage.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Emitter/Emitter.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <iostream>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(PowerFavoringEmitterSampler, EmitterSampler);

void ESPowerFavoring::update(const CookedDataStorage& cookedActors)
{
	m_emitters.clear();
	m_emitters.shrink_to_fit();
	m_distribution      = math::TPwcDistribution1D<real>();
	m_emitterToIndexMap = std::unordered_map<const Emitter*, std::size_t>();

	for(const auto& emitter : cookedActors.emitters())
	{
		m_emitters.push_back(emitter.get());
	}
	PH_LOG(PowerFavoringEmitterSampler, "added {} emitters", m_emitters.size());

	if(m_emitters.empty())
	{
		PH_LOG_WARNING(PowerFavoringEmitterSampler, "no Emitter detected");
		return;
	}

	std::vector<real> sampleWeights(m_emitters.size(), 0);
	for(std::size_t i = 0; i < m_emitters.size(); ++i)
	{
		const Emitter* emitter = m_emitters[i];

		sampleWeights[i] = emitter->calcRadiantFluxApprox();
		m_emitterToIndexMap[emitter] = i;
	}
	m_distribution = math::TPwcDistribution1D<real>(sampleWeights);
}

const Emitter* ESPowerFavoring::pickEmitter(SampleFlow& sampleFlow, real* const out_pdf) const
{
	PH_ASSERT(out_pdf);

	const std::size_t pickedIndex = m_distribution.sampleDiscrete(sampleFlow.flow1D());
	*out_pdf = m_distribution.pdfDiscrete(pickedIndex);
	return m_emitters[pickedIndex];
}

void ESPowerFavoring::genDirectSample(DirectEnergySampleQuery& query, SampleFlow& sampleFlow) const
{
	const std::size_t pickedIndex = m_distribution.sampleDiscrete(sampleFlow.flow1D());// FIXME: use pick
	const real        pickPdf     = m_distribution.pdfDiscrete(pickedIndex);

	m_emitters[pickedIndex]->genDirectSample(query, sampleFlow);
	query.out.pdfW *= pickPdf;
}

real ESPowerFavoring::calcDirectPdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const
{
	const Primitive* hitPrim = emitPos.getDetail().getPrimitive();
	PH_ASSERT(hitPrim);

	const Emitter* hitEmitter = hitPrim->getMetadata()->getSurface().getEmitter();
	PH_ASSERT(hitEmitter);

	const real samplePdfW = hitEmitter->calcDirectSamplePdfW(emitPos, targetPos);

	const auto& result = m_emitterToIndexMap.find(hitEmitter);
	PH_ASSERT(result != m_emitterToIndexMap.end());
	const real pickPdf = m_distribution.pdfDiscrete(result->second);

	return samplePdfW * pickPdf;
}

}// end namespace ph
