#include "World/LightSampler/UniformRandomLightSampler.h"
#include "Actor/AModel.h"
#include "Actor/ALight.h"
#include "Math/Random.h"
#include "Actor/CookedDataStorage.h"
#include "Core/Sample/DirectLightSample.h"
#include "Math/TVector3.h"

#include <iostream>

namespace ph
{

UniformRandomLightSampler::~UniformRandomLightSampler() = default;

void UniformRandomLightSampler::update(const CookedDataStorage& cookedActors)
{
	m_emitters.clear();
	m_emitters.shrink_to_fit();

	for(const auto& emitter : cookedActors.emitters())
	{
		m_emitters.push_back(emitter.get());
	}

	if(m_emitters.empty())
	{
		std::cerr << "warning: at UniformRandomLightSampler::update(), no Emitter detected" << std::endl;
	}
}

const Emitter* UniformRandomLightSampler::pickEmitter(real* const out_PDF) const
{
	const std::size_t picker = static_cast<std::size_t>(Random::genUniformReal_i0_e1() * static_cast<real>(m_emitters.size()));
	const std::size_t pickedIndex = picker == m_emitters.size() ? picker - 1 : picker;

	*out_PDF = 1.0_r / static_cast<real>(m_emitters.size());
	return m_emitters[pickedIndex];
}

void UniformRandomLightSampler::genDirectSample(DirectLightSample& sample) const
{
	// randomly and uniformly select an emitter
	const std::size_t picker = static_cast<std::size_t>(Random::genUniformReal_i0_e1() * static_cast<real>(m_emitters.size()));
	const std::size_t pickedIndex = picker == m_emitters.size() ? picker - 1 : picker;
	const real pickPdfW = 1.0_r / static_cast<real>(m_emitters.size());

	m_emitters[pickedIndex]->genDirectSample(sample);
	sample.pdfW *= pickPdfW;
}

real UniformRandomLightSampler::calcDirectPdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Emitter* hitEmitter, const Primitive* hitPrim) const
{
	const real pickPdfW = 1.0_r / static_cast<real>(m_emitters.size());
	const real samplePdfW = hitEmitter->calcDirectSamplePdfW(targetPos, emitPos, emitN, hitPrim);

	const real samplePdfW2 = hitEmitter->calcDirectSamplePdfW(targetPos, emitPos, emitN, hitPrim);

	return pickPdfW * samplePdfW;
}

}// end namespace ph