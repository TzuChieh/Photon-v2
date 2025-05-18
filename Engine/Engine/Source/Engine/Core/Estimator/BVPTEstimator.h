#pragma once

#include "Engine/Core/Estimator/PathEnergyEstimator.h"

namespace ph
{

/*! @brief A GI estimator using pure path tracing.

BVPT: Backward Vanilla Path Tracing.

This estimator is the basic version of path tracing. Rays shot from
the receiver, bouncing around the scene until an emitter is hit, gather
its emitted power and keep bouncing... Most online tutorials
implemented this GI algorithm. Since this estimator is relatively
simple but still unbiased, it is good for ground truth rendering if
the correctness of another estimator is in doubt.
*/
class BVPTEstimator : public PathEnergyEstimator
{
public:
	void setPTParams(PTEstimatorParams params) override;

	void update(const Integrand& integrand) override;

	void estimate(
		const Ray&        ray,
		const Integrand&  integrand,
		SampleFlow&       sampleFlow,
		EnergyEstimation& out_estimation) override;

	std::unique_ptr<TIRayEstimator<math::Spectrum>> makeCopy() const override;

	std::string toString() const override;
};

}// end namespace ph
