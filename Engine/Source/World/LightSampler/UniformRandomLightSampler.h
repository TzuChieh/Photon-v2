#pragma once

#include "World/LightSampler/LightSampler.h"
#include "Core/Emitter/EmitterStorage.h"

#include <vector>

namespace ph
{

class UniformRandomLightSampler : public LightSampler
{
public:
	virtual ~UniformRandomLightSampler() override;

	virtual void update(const EmitterStorage& emitters) override;
	virtual const Emitter* pickEmitter(float32* const out_PDF) const override;

private:
	std::vector<const Emitter*> m_emitters;
};

}// end namespace ph