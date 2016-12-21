#include "World/LightSampler/UniformRandomLightSampler.h"
#include "Entity/Entity.h"
#include "Entity/Light/Light.h"

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