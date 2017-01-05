#include "World/LightSampler/UniformRandomLightSampler.h"
#include "Actor/AModel.h"
#include "Actor/ALight.h"
#include "Math/random_number.h"
#include "Core/CookedActorStorage.h"
#include "Core/Sample/DirectLightSample.h"

#include <iostream>

namespace ph
{

UniformRandomLightSampler::~UniformRandomLightSampler() = default;

void UniformRandomLightSampler::update(const CookedActorStorage& cookedActors)
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

const Emitter* UniformRandomLightSampler::pickEmitter(float32* const out_PDF) const
{
	const std::size_t picker = static_cast<std::size_t>(genRandomFloat32_0_1_uniform() * static_cast<float32>(m_emitters.size()));
	const std::size_t pickedIndex = picker == m_emitters.size() ? picker - 1 : picker;

	*out_PDF = 1.0f / static_cast<float32>(m_emitters.size());
	return m_emitters[pickedIndex];
}

void UniformRandomLightSampler::genDirectSample(DirectLightSample& sample) const
{
	// randomly and uniformly select an emitter
	const std::size_t picker = static_cast<std::size_t>(genRandomFloat32_0_1_uniform() * static_cast<float32>(m_emitters.size()));
	const std::size_t pickedIndex = picker == m_emitters.size() ? picker - 1 : picker;
	const float32 pickPdfW = 1.0f / static_cast<float32>(m_emitters.size());

	m_emitters[pickedIndex]->genDirectSample(sample);
	sample.pdfW *= pickPdfW;
}

float32 UniformRandomLightSampler::calcDirectPdfW(const Vector3f& targetPos, const Vector3f& emitPos, const Vector3f& emitN, const Emitter* hitEmitter, const Primitive* hitPrim) const
{
	const float32 pickPdfW = 1.0f / static_cast<float32>(m_emitters.size());
	const float32 samplePdfW = hitEmitter->calcDirectSamplePdfW(targetPos, emitPos, emitN, hitPrim);
	return pickPdfW * samplePdfW;
}

}// end namespace ph