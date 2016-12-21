#pragma once

#include "World/LightSampler/LightSampler.h"
#include "Core/Emitter/EmitterStorage.h"

namespace ph
{

class UniformRandomLightSampler : public LightSampler
{
public:
	virtual ~UniformRandomLightSampler() override;

	virtual void update(const std::vector<Entity>& entities) override;

private:
	EmitterStorage m_emitterStorage;

	void gatherEmittersFromEntity(const Entity& entity);
	static bool matchLightSignature(const Entity& entity);
};

}// end namespace ph