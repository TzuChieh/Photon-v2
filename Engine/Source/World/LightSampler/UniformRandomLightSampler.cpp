#include "World/LightSampler/UniformRandomLightSampler.h"
#include "Entity/Entity.h"
#include "Entity/Light/Light.h"
#include "Math/random_number.h"

#include <iostream>

namespace ph
{

UniformRandomLightSampler::~UniformRandomLightSampler() = default;

void UniformRandomLightSampler::update(const std::vector<Entity>& entities)
{
	m_emitterStorage.clear();

	for(const auto& entity : entities)
	{
		gatherEmittersFromEntity(entity);
	}
}

const Emitter* UniformRandomLightSampler::pickEmitter(float32* const out_PDF) const
{
	if(m_emitterStorage.numEmitters() == 0)
	{
		return nullptr;
	}

	const std::size_t picker = static_cast<std::size_t>(genRandomFloat32_0_1_uniform() * static_cast<float32>(m_emitterStorage.numEmitters()));
	const std::size_t pickedIndex = picker == m_emitterStorage.numEmitters() ? picker - 1 : picker;

	*out_PDF = 1.0f / static_cast<float32>(m_emitterStorage.numEmitters());
	return &(m_emitterStorage[pickedIndex]);
}

void UniformRandomLightSampler::gatherEmittersFromEntity(const Entity& entity)
{
	if(matchLightSignature(entity))
	{
		entity.getLight()->buildEmitters(&m_emitterStorage, entity);
	}
	
	for(const auto& child : entity.getChildren())
	{
		gatherEmittersFromEntity(child);
	}
}

bool UniformRandomLightSampler::matchLightSignature(const Entity& entity)
{
	return entity.getLight() && entity.getLocalToWorldTransform() && entity.getWorldToLocalTransform();
}

}// end namespace ph