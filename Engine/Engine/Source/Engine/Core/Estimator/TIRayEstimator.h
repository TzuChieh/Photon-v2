#pragma once

#include "Engine/Core/Estimator/estimator_fwd.h"
#include "Engine/Core/Estimator/TEstimationArray.h"

#include <Common/logging.h>

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

	/*! @brief Update according to the given integrand and the parameters set.
	*/
	virtual void update(const Integrand& integrand) = 0;

	/*! @brief Perform estimation.
	Should be called after `update()`. If the integrand and the parameters have not changed, multiple
	estimates can be performed after `update()`.
	*/
	virtual void estimate(
		const Ray&                        ray,
		const Integrand&                  integrand,
		SampleFlow&                       sampleFlow,
		TEstimationArray<EstimationType>& out_estimation) = 0;

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
{
	PH_DEFAULT_LOG(Warning,
		"at TIRayEstimator::setEstimationIndex(), "
		"attempting to set estimation index for unsupported attribute");
}

template<typename EstimationType>
inline std::string TIRayEstimator<EstimationType>::toString() const
{
	return "Ray Estimator";
}

}// end namespace ph
