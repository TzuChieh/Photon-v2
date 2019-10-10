#pragma once

#include "Core/Estimator/TIRayEstimator.h"
#include "Math/TVector3.h"

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
		TEstimationArray<math::Vector3R>& out_estimation) const override;

	virtual std::string toString() const;
};

// In-header Implementations:

inline std::string SurfaceAttributeEstimator::toString() const
{
	return "Surface Attribute Estimator";
}

}// end namespace ph
