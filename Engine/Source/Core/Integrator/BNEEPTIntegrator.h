#pragma once

#include "Common/primitive_type.h"
#include "Core/Integrator/Integrator.h"
#include "Math/math_fwd.h"

namespace ph
{

/*
	BNEEPT: Backward Next Event Estimation Path Tracing

	A slightly advanced rendering algorithm that provides faster convergence
	over BVPT. The basic concept is the same as BVPT, except that on each 
	intersection point, an additional ray is shot towards an emitter and its
	lighting contribution is accounted unbiasedly. This integrator works well 
	on small lights while BVPT does poorly.
*/
class BNEEPTIntegrator final : public Integrator, public TCommandInterface<BNEEPTIntegrator>
{
public:
	virtual ~BNEEPTIntegrator() override;

	virtual void update(const Scene& scene) override;
	virtual void radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const override;

private:
	static void rationalClamp(SpectralStrength& value);
	static real misWeight(real pdf1W, real pdf2W);

// command interface
public:
	BNEEPTIntegrator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<BNEEPTIntegrator> ciLoad(const InputPacket& packet);
};

}// end namespace ph