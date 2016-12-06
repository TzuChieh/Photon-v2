#include "BackwardPathIntegrator.h"
#include "Core/Ray.h"
#include "World/World.h"
#include "Math/Vector3f.h"
#include "Core/Intersection.h"
#include "Model/Model.h"
#include "Model/Material/Material.h"
#include "Model/Material/Integrand/SurfaceIntegrand.h"
#include "Math/Math.h"
#include "Math/Color.h"
#include "Math/random_number.h"
#include "Model/Geometry/Triangle.h"

#include <iostream>

#define MAX_RAY_BOUNCES 10000

namespace ph
{

BackwardPathIntegrator::~BackwardPathIntegrator() = default;

void BackwardPathIntegrator::update(const Intersector& intersector)
{
	// update nothing
}

void BackwardPathIntegrator::radianceAlongRay(const Ray& ray, const Intersector& intersector, Vector3f* const out_radiance) const
{
	uint32 numBounces = 0;
	Vector3f accuRadiance(0, 0, 0);
	Vector3f accuLiWeight(1, 1, 1);
	Intersection intersection;
	Ray tracingRay(ray);

	while(numBounces <= MAX_RAY_BOUNCES && intersector.isIntersecting(tracingRay, &intersection))
	{
		const Model* hitModel = intersection.getHitTriangle()->getParentModel();
		const Material* hitMaterial = hitModel->getMaterial();

		const Vector3f N(intersection.getHitNormal());
		const Vector3f V(tracingRay.getDirection().mul(-1.0f));

		SurfaceSample surfaceSample;
		hitMaterial->getSurfaceIntegrand()->evaluateImportanceSample(intersection, tracingRay, &surfaceSample);
		bool keepSampling = true;
		switch(surfaceSample.m_type)
		{
		case ESurfaceSampleType::EMISSION:
		{
			Vector3f radianceLi = surfaceSample.m_emittedRadiance;

			// avoid excessive, negative weight and possible NaNs
			accuLiWeight.clampLocal(0.0f, 1000.0f);

			accuRadiance.addLocal(radianceLi.mul(accuLiWeight));
			keepSampling = false;
			break;
		}
		break;

		case ESurfaceSampleType::REFLECTION:
		case ESurfaceSampleType::TRANSMISSION:
		{
			Vector3f liWeight = surfaceSample.m_LiWeight;

			//const float32 rrSurviveRate = liWeight.clamp(0.0f, 1.0f).max();
			const float32 rrSurviveRate = Math::clamp(liWeight.avg(), 0.0001f, 1.0f);
			//const float32 rrSurviveRate = Math::clamp(Color::linearRgbLuminance(liWeight), 0.0001f, 1.0f);
			const float32 rrSpin = genRandomFloat32_0_1_uniform();

			// russian roulette >> survive
			if(rrSurviveRate > rrSpin)
			{
				const float32 rrScale = 1.0f / rrSurviveRate;
				liWeight.mulLocal(rrScale);
			}
			// russian roulette >> dead
			else
			{
				keepSampling = false;
			}

			accuLiWeight.mulLocal(liWeight);
			break;
		}
		break;

		default:
			std::cerr << "warning: unknown surface sample type in BackwardPathIntegrator detected" << std::endl;
			keepSampling = false;
			break;
		}// end switch surface sample type

		if(!keepSampling)
		{
			break;
		}

		// prepare for next recursion
		const Vector3f L = surfaceSample.m_direction;
		const Vector3f nextRayOrigin(intersection.getHitPosition().add(N.mul(0.0001f)));
		const Vector3f nextRayDirection(L);
		tracingRay.setOrigin(nextRayOrigin);
		tracingRay.setDirection(nextRayDirection);
		numBounces++;
		intersection.clear();
	}// end while

	*out_radiance = accuRadiance;
}

}// end namespace ph