#pragma once

#include "Engine/Core/Estimator/TIRayEstimator.h"
#include "Engine/Math/TVector3.h"

namespace ph
{

class SurfaceHit;

class SurfaceAttributeEstimator : public TIRayEstimator<math::Vector3R>
{
public:
	void update(const Integrand& integrand) override;

	void estimate(
		const Ray&                        ray,
		const Integrand&                  integrand,
		SampleFlow&                       sampleFlow,
		TEstimationArray<math::Vector3R>& out_estimation) const override;

	std::unique_ptr<TIRayEstimator<math::Vector3R>> makeCopy() const;

	std::string toString() const override;
};

}// end namespace ph
