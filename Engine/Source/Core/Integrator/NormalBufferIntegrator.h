#pragma once

#include "Core/Integrator/Integrator.h"

namespace ph
{

class NormalBufferIntegrator final : Integrator
{
public:
	virtual ~NormalBufferIntegrator() override;

	virtual void update(const World& world) override;
	virtual void radianceAlongRay(const Sample& sample, const World& world, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const override;
};

}// end namespace ph