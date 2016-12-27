#include "World/LightSampler/UniformRandomLightSampler.h"
#include "Actor/Model/Model.h"
#include "Actor/Light/Light.h"
#include "Math/random_number.h"
#include "Core/CookedLightStorage.h"

#include <iostream>

namespace ph
{

UniformRandomLightSampler::~UniformRandomLightSampler() = default;

void UniformRandomLightSampler::update(const CookedLightStorage& cookedLightStorage)
{
	m_emitters.clear();
	m_emitters.shrink_to_fit();

	for(const auto& emitter : cookedLightStorage)
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

}// end namespace ph