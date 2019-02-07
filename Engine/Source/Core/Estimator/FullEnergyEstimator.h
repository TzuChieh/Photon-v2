#pragma once

#include "Core/Estimator/IEnergyEstimator.h"

#include <cstddef>

namespace ph
{

class FullEnergyEstimator : public IEnergyEstimator
{
public:
	void update(const Integrand& integrand) override = 0;

	void estimate(
		const Ray&        ray,
		const Integrand&  integrand,
		EnergyEstimation& out_estimation) const override = 0;

	void setEstimationIndex(std::size_t index);

protected:
	std::size_t m_estimationIndex;
};

// In-header Implementations:

inline void FullEnergyEstimator::setEstimationIndex(const std::size_t index)
{
	m_estimationIndex = index;
}

}// end namespace ph