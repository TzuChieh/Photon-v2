#pragma once

#include "Core/Estimator/TEstimationArray.h"

#include <string>

namespace ph
{

class Ray;
class Integrand;

template<typename EstimationType>
class TIRayEstimator
{
public:
	virtual ~TIRayEstimator() = default;

	virtual void update(const Integrand& integrand) = 0;

	virtual void estimate(
		const Ray&                        ray, 
		const Integrand&                  integrand, 
		TEstimationArray<EstimationType>& out_estimation) const = 0;

	virtual std::string toString() const;
};

// In-header Implementations:

template<typename EstimationType>
inline std::string TIRayEstimator<EstimationType>::toString() const
{
	return "Ray Estimator";
}

}// end namespace ph