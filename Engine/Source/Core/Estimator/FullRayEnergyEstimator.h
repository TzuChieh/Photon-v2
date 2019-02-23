#pragma once

#include "Core/Estimator/IRayEnergyEstimator.h"

#include <cstddef>

namespace ph
{

class FullRayEnergyEstimator : public IRayEnergyEstimator
{
public:
	void update(const Integrand& integrand) override = 0;

	void estimate(
		const Ray&        ray,
		const Integrand&  integrand,
		EnergyEstimation& out_estimation) const override = 0;

	void setEstimationIndex(std::size_t index);

	std::string toString() const override;

protected:
	std::size_t m_estimationIndex;
};

// In-header Implementations:

inline void FullRayEnergyEstimator::setEstimationIndex(const std::size_t index)
{
	m_estimationIndex = index;
}

inline std::string FullRayEnergyEstimator::toString() const
{
	return "Full Ray Energy Estimator";
}

}// end namespace ph