#include "Core/Emitter/Sampler/ESPowerFavoring.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/Emitter/Query/DirectEnergyPdfQuery.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Emitter/Emitter.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <cmath>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(PowerFavoringEmitterSampler, EmitterSampler);

void ESPowerFavoring::update(TSpanView<const Emitter*> emitters)
{
	m_emitters.clear();
	m_emitters.shrink_to_fit();
	m_distribution      = math::TPwcDistribution1D<real>();
	m_emitterToIndexMap = std::unordered_map<const Emitter*, std::size_t>();

	for(const Emitter* emitter : emitters)
	{
		m_emitters.push_back(emitter);
	}
	PH_LOG(PowerFavoringEmitterSampler, Note, "added {} emitters", m_emitters.size());

	if(m_emitters.empty())
	{
		PH_LOG(PowerFavoringEmitterSampler, Warning, "no Emitter detected");
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

void ESPowerFavoring::genDirectSample(
	DirectEnergySampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	const std::size_t pickedIndex = m_distribution.sampleDiscrete(sampleFlow.flow1D());// FIXME: use pick
	const real        pickPdf     = m_distribution.pdfDiscrete(pickedIndex);

	m_emitters[pickedIndex]->genDirectSample(query, sampleFlow, probe);
	if(!query.outputs)
	{
		return;
	}

	query.outputs.setPdf(query.outputs.getPdf() * pickPdf);
}

void ESPowerFavoring::calcDirectPdf(DirectEnergyPdfQuery& query) const
{
	const Primitive& hitPrim = query.inputs.getSrcPrimitive();
	const Emitter* hitEmitter = hitPrim.getMetadata()->getSurface().getEmitter();
	if(!hitEmitter)
	{
		query.outputs.setPdf({});
		return;
	}

	hitEmitter->calcDirectPdf(query);
	if(!query.outputs)
	{
		return;
	}

	const auto& result = m_emitterToIndexMap.find(hitEmitter);
	PH_ASSERT(result != m_emitterToIndexMap.end());
	const real pickPdf = m_distribution.pdfDiscrete(result->second);
	query.outputs.setPdf(query.outputs.getPdf() * pickPdf);
}

}// end namespace ph
