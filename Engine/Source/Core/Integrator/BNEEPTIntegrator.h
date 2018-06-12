#pragma once

#include "Common/primitive_type.h"
#include "Core/Integrator/AbstractPathIntegrator.h"
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

	A good reference on this technique is Fabio Pellacini's awesome lecture
	on path tracing.
	His page:     http://pellacini.di.uniroma1.it/
	Lecture Note: http://pellacini.di.uniroma1.it/teaching/graphics08/lectures/18_PathTracing_Web.pdf
*/
class BNEEPTIntegrator final : public AbstractPathIntegrator, public TCommandInterface<BNEEPTIntegrator>
{
public:
	BNEEPTIntegrator();
	BNEEPTIntegrator(const BNEEPTIntegrator& other);

	std::unique_ptr<Integrator> makeReproduction() const override;

	BNEEPTIntegrator& operator = (BNEEPTIntegrator rhs);

	friend void swap(BNEEPTIntegrator& first, BNEEPTIntegrator& second);

private:
	void tracePath(
		const Ray&        ray,
		SpectralStrength* out_lightEnergy,
		SurfaceHit*       out_firstHit) const override;

	static void rationalClamp(SpectralStrength& value);

// command interface
public:
	BNEEPTIntegrator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<BNEEPTIntegrator> ciLoad(const InputPacket& packet);
};

}// end namespace ph