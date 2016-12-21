#pragma once

#include "World/LightSampler/LightSampler.h"

namespace ph
{

class UniformRandomLightSampler : public LightSampler
{
public:
	virtual ~UniformRandomLightSampler() override;

	virtual void update(const std::vector<Entity>& entities) override;
};

}// end namespace ph