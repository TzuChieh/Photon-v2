#pragma once

namespace ph
{

class Ray;
class Integrand;
class EnergyEstimation;

class IRayEnergyEstimator
{
public:
	virtual ~IRayEnergyEstimator() = default;

	virtual void update(const Integrand& integrand) = 0;

	virtual void estimate(
		const Ray&        ray, 
		const Integrand&  integrand, 
		EnergyEstimation& out_estimation) const = 0;
};

}// end namespace ph