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
	virtual const Emitter* pickEmitter(float32* const out_PDF) const override;

private:
	EmitterStorage m_emitterStorage;

	void gatherEmittersFromEntity(const Entity& entity);
	static bool matchLightSignature(const Entity& entity);
};

}// end namespace ph