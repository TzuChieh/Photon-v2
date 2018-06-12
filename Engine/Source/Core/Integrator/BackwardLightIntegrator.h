#pragma once

#include "Core/Integrator/Integrator.h"
#include "Math/math_fwd.h"

namespace ph
{

class BackwardLightIntegrator final : public Integrator, public TCommandInterface<BackwardLightIntegrator>
{
public:
	virtual ~BackwardLightIntegrator() override;

	/*virtual void update(const Scene& scene) override;
	virtual void radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const override;*/

private:
	static void rationalClamp(SpectralStrength& value);

// command interface
public:
	BackwardLightIntegrator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<BackwardLightIntegrator> ciLoad(const InputPacket& packet);
};

}// end namespace ph