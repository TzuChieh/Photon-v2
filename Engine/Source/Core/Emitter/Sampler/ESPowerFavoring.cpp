#include "Core/Emitter/Sampler/ESPowerFavoring.h"
#include "Math/Random.h"
#include "Actor/CookedDataStorage.h"
#include "Core/Sample/DirectLightSample.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Emitter/Emitter.h"
#include "Common/assertion.h"
#include "Common/Logger.h"

#include <iostream>

namespace ph
{

namespace
{
	const Logger logger(LogSender("Emitter Sampler: Power Favoring"));
}

void ESPowerFavoring::update(const CookedDataStorage& cookedActors)
{
	m_emitters.clear();
	m_emitters.shrink_to_fit();
	m_distribution      = TPwcDistribution1D<real>();
	m_emitterToIndexMap = std::unordered_map<const Emitter*, std::size_t>();

	for(const auto& emitter : cookedActors.emitters())
	{
		m_emitters.push_back(emitter.get());
	}
	logger.log("added " + std::to_string(m_emitters.size()) + " emitters");

	if(m_emitters.empty())
	{
		logger.log(ELogLevel::WARNING_MED, "no Emitter detected");
		return;
	}

	std::vector<real> sampleWeights(m_emitters.size(), 0);
	for(std::size_t i = 0; i < m_emitters.size(); ++i)
	{
		const Emitter* emitter = m_emitters[i];

		sampleWeights[i] = emitter->calcRadiantFluxApprox();
		m_emitterToIndexMap[emitter] = i;
	}
	m_distribution = TPwcDistribution1D<real>(sampleWeights);
}

const Emitter* ESPowerFavoring::pickEmitter(real* const out_pdf) const
{
	PH_ASSERT(out_pdf);

	const std::size_t pickedIndex = m_distribution.sampleDiscrete(Random::genUniformReal_i0_e1());
	*out_pdf = m_distribution.pdfDiscrete(pickedIndex);
	return m_emitters[pickedIndex];
}

void ESPowerFavoring::genDirectSample(DirectLightSample& sample) const
{
	const std::size_t pickedIndex = m_distribution.sampleDiscrete(Random::genUniformReal_i0_e1());
	const real        pickPdf     = m_distribution.pdfDiscrete(pickedIndex);

	m_emitters[pickedIndex]->genDirectSample(sample);
	sample.pdfW *= pickPdf;
}

real ESPowerFavoring::calcDirectPdfW(const SurfaceHit& emitPos, const Vector3R& targetPos) const
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