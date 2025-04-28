#pragma once

#include "Engine/Core/Estimator/estimator_fwd.h"
#include "Engine/Core/Estimator/TEstimationArray.h"

#include <string>
#include <memory>

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

	/*! @brief Map an attribute to an index in the estimation array.
	*/
	virtual void setEstimationIndex(
		EEstimatorAttribute attribute, 
		int estimationIdx);

	/*! @brief Create a copy of this estimator.
	*/
	virtual std::unique_ptr<TIRayEstimator> makeCopy() const = 0;

	/*!
	@return General information of the estimator. Useful for debug purposes.
	*/
	virtual std::string toString() const;
};

// In-header Implementations:

template<typename EstimationType>
inline void TIRayEstimator<EstimationType>::setEstimationIndex(
	const EEstimatorAttribute attribute,
	const int estimationIdx)
{}

template<typename EstimationType>
inline std::string TIRayEstimator<EstimationType>::toString() const
{
	return "Ray Estimator";
}

}// end namespace ph
