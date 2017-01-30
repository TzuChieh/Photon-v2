#pragma once

#include "Core/Integrator/Integrator.h"
#include "Math/math_fwd.h"

namespace ph
{

class BackwardLightIntegrator final : public Integrator
{
public:
	BackwardLightIntegrator(const InputPacket& packet);
	virtual ~BackwardLightIntegrator() override;

	virtual void update(const World& world) override;
	virtual void radianceAlongRay(const Sample& sample, const World& world, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const override;

private:
	static void rationalClamp(Vector3R& value);
};

}// end namespace ph