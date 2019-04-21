#include "Core/Estimator/SurfaceAttributeEstimator.h"
#include "Core/Ray.h"
#include "Core/Estimator/Integrand.h"
#include "Core/SurfaceHit.h"
#include "Core/HitProbe.h"
#include "World/Scene.h"
#include "Common/assertion.h"
#include "Math/Mapping/CosThetaWeightedUnitHemisphere.h"
#include "Math/Random.h"
#include "Math/constant.h"

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

		if(out_estimation.numEstimations() >= 1)
		{
			out_estimation[0] = surfaceHit.getShadingNormal();
		}

		if(out_estimation.numEstimations() >= 2)
		{
			out_estimation[1] = surfaceHit.getGeometryNormal();
		}
		
		if(out_estimation.numEstimations() >= 3)
		{
			out_estimation[2] = surfaceHit.getPosition();
		}

		if(out_estimation.numEstimations() >= 4)
		{
			out_estimation[3] = surfaceHit.getDetail().getUvw();
		}

		if(out_estimation.numEstimations() >= 5)
		{
			out_estimation[4] = Vector3R(surfaceHit.getDetail().getRayT());
		}

		if(out_estimation.numEstimations() >= 6)
		{
			constexpr std::size_t NUM_AO_SAMPLES = 64;

			const Vector3R aoAlbedo(1.0_r);

			real estimation = 0;
			for(std::size_t i = 0; i < NUM_AO_SAMPLES; ++i)
			{
				real pdfW;
				Vector3R L = CosThetaWeightedUnitHemisphere::map(
					{Random::genUniformReal_i0_e1(), Random::genUniformReal_i0_e1()},
					&pdfW);
				L = surfaceHit.getDetail().getShadingBasis().localToWorld(L);

				const Ray aoRay(surfaceHit.getPosition(), L, 0.001_r, std::numeric_limits<real>::max());
				if(integrand.getScene().isIntersecting(aoRay))
				{
					estimation += 1 / pdfW;
				}
			}
			estimation /= static_cast<real>(NUM_AO_SAMPLES);
			estimation /= constant::pi<real>;

			out_estimation[5] = aoAlbedo * estimation;
		}
	}
	else
	{
		for(std::size_t i = 0; i < out_estimation.numEstimations(); ++i)
		{
			out_estimation[i] = Vector3R(0);
		}
	}
}

}// end namespace ph