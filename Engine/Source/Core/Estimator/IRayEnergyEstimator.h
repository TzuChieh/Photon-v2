#pragma once

#include "Core/Estimator/TIRayEstimator.h"
#include "Math/Color/Spectrum.h"

namespace ph
{

class IRayEnergyEstimator : public TIRayEstimator<math::Spectrum>
{
public:
	using EnergyEstimation = TEstimationArray<math::Spectrum>;

	void update(const Integrand& integrand) override = 0;

	void estimate(
		const Ray&        ray, 
		const Integrand&  integrand, 
		SampleFlow&       sampleFlow,
		EnergyEstimation& out_estimation) const override = 0;

	std::string toString() const override;
};

// In-header Implementations:

inline std::string IRayEnergyEstimator::toString() const
{
	return "Ray Energy Estimator";
}

}// end namespace ph
