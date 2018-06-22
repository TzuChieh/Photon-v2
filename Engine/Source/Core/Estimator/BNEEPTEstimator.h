#pragma once

#include "Common/primitive_type.h"
#include "Core/Estimator/Estimator.h"
#include "Math/math_fwd.h"

namespace ph
{

/*
	BNEEPT: Backward Next Event Estimation Path Tracing

	A slightly advanced rendering algorithm that provides faster convergence
	over BVPT. The basic concept is the same as BVPT, except that on each 
	intersection point, an additional ray is shot towards an emitter and its
	lighting contribution is accounted unbiasedly. This estimator works well 
	on small lights while BVPT does poorly.

	A good reference on this technique is Fabio Pellacini's awesome lecture
	on path tracing.
	His page:     http://pellacini.di.uniroma1.it/
	Lecture Note: http://pellacini.di.uniroma1.it/teaching/graphics08/lectures/18_PathTracing_Web.pdf
*/
class BNEEPTEstimator final : public Estimator, public TCommandInterface<BNEEPTEstimator>
{
public:
	BNEEPTEstimator();
	virtual ~BNEEPTEstimator() override;

	virtual void update(const Scene& scene) override;
	virtual void radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const override;

private:
	static void rationalClamp(SpectralStrength& value);

// command interface
public:
	BNEEPTEstimator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<BNEEPTEstimator> ciLoad(const InputPacket& packet);
};

}// end namespace ph