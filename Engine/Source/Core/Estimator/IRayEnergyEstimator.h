#pragma once

#include "Core/Estimator/TIRayEstimator.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class IRayEnergyEstimator : public TIRayEstimator<SpectralStrength>
{
public:
	using EnergyEstimation = TEstimationArray<SpectralStrength>;

	void update(const Integrand& integrand) override = 0;

	void estimate(
		const Ray&        ray, 
		const Integrand&  integrand, 
		EnergyEstimation& out_estimation) const override = 0;

	std::string toString() const override;
};

// In-header Implementations:

inline std::string IRayEnergyEstimator::toString() const
{
	return "Ray Energy Estimator";
}

}// end namespace ph