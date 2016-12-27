#pragma once

#include "World/LightSampler/LightSampler.h"

#include <vector>

namespace ph
{

class UniformRandomLightSampler : public LightSampler
{
public:
	virtual ~UniformRandomLightSampler() override;

	virtual void update(const CookedLightStorage& cookedLightStorage) override;
	virtual const Emitter* pickEmitter(float32* const out_PDF) const override;

private:
	std::vector<const Emitter*> m_emitters;
};

}// end namespace ph