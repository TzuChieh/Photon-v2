#pragma once

#include "Core/Estimator/FullRayEnergyEstimator.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

/*
	BNEEPT: Backward Next Event Estimation Path Tracing

	A slightly advanced rendering algorithm that provides faster convergence
	over BVPT. The basic concept is the same as BVPT, except that on each 
	intersection point, an additional ray is shot towards an emitter and its
	lighting contribution is accounted unbiasedly. This estimator works well 
	on small lights while BVPT does poorly.

	Reference:

	A good reference on this technique is Fabio Pellacini's awesome lecture
	on path tracing.
	His page:     http://pellacini.di.uniroma1.it/
	Lecture Note: http://pellacini.di.uniroma1.it/teaching/graphics08/lectures/18_PathTracing_Web.pdf
*/
class BNEEPTEstimator : public FullRayEnergyEstimator
{
public:
	void update(const Integrand& integrand) override;

	void estimate(
		const Ray&        ray,
		const Integrand&  integrand,
		EnergyEstimation& out_estimation) const override;

private:
	static void rationalClamp(SpectralStrength& value);
};

// In-header Implementations:

inline void BNEEPTEstimator::update(const Integrand& integrand)
{}

}// end namespace ph