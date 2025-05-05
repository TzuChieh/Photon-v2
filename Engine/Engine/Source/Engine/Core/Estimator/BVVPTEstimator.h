#pragma once

#include "Engine/Core/Estimator/PathEnergyEstimator.h"

namespace ph
{

/*! @brief A GI estimator using pure path tracing.

BVVPT: Backward Vanilla Volumetric Path Tracing.

In addition to `BVPTEstimator`, this estimator also handles volumetric light transport.
*/
class BVVPTEstimator : public PathEnergyEstimator
{
public:
	void update(const Integrand& integrand) override;

	void estimate(
		const Ray&        ray,
		const Integrand&  integrand,
		SampleFlow&       sampleFlow,
		EnergyEstimation& out_estimation) override;

	std::unique_ptr<TIRayEstimator<math::Spectrum>> makeCopy() const override = 0;

	std::string toString() const override;
};

}// end namespace ph
