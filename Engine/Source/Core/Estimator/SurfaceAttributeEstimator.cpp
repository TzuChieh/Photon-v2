#include "Core/Estimator/SurfaceAttributeEstimator.h"
#include "Core/Ray.h"
#include "Core/Estimator/Integrand.h"
#include "Core/SurfaceHit.h"
#include "Core/HitProbe.h"
#include "World/Scene.h"
#include "Common/assertion.h"

#include <limits>

namespace ph
{

void SurfaceAttributeEstimator::update(const Integrand& integrand)
{}

void SurfaceAttributeEstimator::estimate(
	const Ray&                  ray,
	const Integrand&            integrand,
	TEstimationArray<Vector3R>& out_estimation) const
{
	Ray probingRay = Ray(ray).reverse();
	probingRay.setMaxT(std::numeric_limits<real>::max());

	HitProbe probe;
	if(integrand.getScene().isIntersecting(probingRay, &probe))
	{
		SurfaceHit surfaceHit(probingRay, probe);

		out_estimation[0] = surfaceHit.getShadingNormal();
	}
	else
	{
		out_estimation[0] = Vector3R(0);
	}
}

}// end namespace ph