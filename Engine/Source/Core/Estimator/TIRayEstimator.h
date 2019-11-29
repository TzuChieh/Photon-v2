#pragma once

#include "Core/Estimator/TEstimationArray.h"

#include <string>
#include <cstddef>

namespace ph
{

class Ray;
class Integrand;
class SampleFlow;

template<typename EstimationType>
class TIRayEstimator
{
public:
	virtual ~TIRayEstimator() = default;

	virtual void update(const Integrand& integrand) = 0;

	virtual void estimate(
		const Ray&                        ray, 
		const Integrand&                  integrand, 
		SampleFlow&                       sampleFlow,
		TEstimationArray<EstimationType>& out_estimation) const = 0;

	virtual void mapAttributeToEstimation(
		std::size_t attributeId, 
		std::size_t estimationIndex);

	virtual std::string toString() const;
};

// In-header Implementations:

template<typename EstimationType>
inline void TIRayEstimator<EstimationType>::mapAttributeToEstimation(
	const std::size_t attributeId,
	const std::size_t estimationIndex)
{}

template<typename EstimationType>
inline std::string TIRayEstimator<EstimationType>::toString() const
{
	return "Ray Estimator";
}

}// end namespace ph
