#pragma once

#include "Common/primitive_type.h"
#include "Core/Integrator/Integrator.h"
#include "Math/math_fwd.h"

namespace ph
{

class BackwardMisIntegrator final : public Integrator
{
public:
	BackwardMisIntegrator(const InputPacket& packet);
	virtual ~BackwardMisIntegrator() override;

	virtual void update(const World& world) override;
	virtual void radianceAlongRay(const Sample& sample, const World& world, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const override;

private:
	static void rationalClamp(Vector3R& value);
	static real misWeight(real pdf1W, real pdf2W);
};

}// end namespace ph