#pragma once

#include "Core/Integrator/Integrator.h"

namespace ph
{

class BackwardPathIntegrator final : public Integrator
{
public:
	BackwardPathIntegrator(const InputPacket& packet);
	virtual ~BackwardPathIntegrator() override;

	virtual void update(const Scene& scene) override;
	virtual void radianceAlongRay(const Sample& sample, const Scene& scene, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const override;
};

}// end namespace ph