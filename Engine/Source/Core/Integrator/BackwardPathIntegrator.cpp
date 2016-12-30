#include "Core/Integrator/BackwardPathIntegrator.h"
#include "Core/Ray.h"
#include "World/World.h"
#include "World/Intersector.h"
#include "World/LightSampler/LightSampler.h"
#include "Math/Vector3f.h"
#include "Core/Intersection.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/BSDFcos.h"
#include "Core/SurfaceBehavior/SurfaceSample.h"
#include "Core/SurfaceBehavior/ESurfaceSampleType.h"
#include "Math/Math.h"
#include "Math/Color.h"
#include "Math/random_number.h"
#include "Core/Primitive/Primitive.h"
#include "Core/Emitter/Emitter.h"

#include <iostream>

#define MAX_RAY_BOUNCES 10000

namespace ph
{

BackwardPathIntegrator::~BackwardPathIntegrator() = default;

void BackwardPathIntegrator::update(const World& world)
{
	// update nothing
}

void BackwardPathIntegrator::radianceAlongRay(const Ray& ray, const World& world, Vector3f* const out_radiance) const
{
	const float32 rayDeltaDist = 0.0001f;
	const Intersector& intersector = world.getIntersector();

	uint32 numBounces = 0;
	Vector3f accuRadiance(0, 0, 0);
	Vector3f accuLiWeight(1, 1, 1);
	Vector3f rayOriginDelta;
	Intersection intersection;

	// backward tracing to light
	Ray tracingRay(ray.getOrigin(), ray.getDirection().mul(-1.0f), RAY_T_EPSILON, RAY_T_MAX);

	const Primitive* lastPrimitive = nullptr;

	while(numBounces <= MAX_RAY_BOUNCES && intersector.isIntersecting(tracingRay, &intersection))
	{
		bool keepSampling = true;

		const auto* const metadata = intersection.getHitPrimitive()->getMetadata();
		const SurfaceBehavior& hitSurfaceBehavior = metadata->surfaceBehavior;

		Vector3f L;

		///////////////////////////////////////////////////////////////////////////////
		// sample emitted radiance

		if(hitSurfaceBehavior.getEmitter())
		{
			Vector3f radianceLi;
			hitSurfaceBehavior.getEmitter()->evalEmittedRadiance(intersection, &radianceLi);

			// avoid excessive, negative weight and possible NaNs
			//accuLiWeight.clampLocal(0.0f, 1000.0f);

			// avoid excessive, negative weight and possible NaNs
			accuLiWeight.x = accuLiWeight.x > 0.0f && accuLiWeight.x < 10000.0f ? accuLiWeight.x : 0.0f;
			accuLiWeight.y = accuLiWeight.y > 0.0f && accuLiWeight.y < 10000.0f ? accuLiWeight.y : 0.0f;
			accuLiWeight.z = accuLiWeight.z > 0.0f && accuLiWeight.z < 10000.0f ? accuLiWeight.z : 0.0f;
			if(accuLiWeight.squaredLength() == 0.0f)
			{
				break;
			}

			accuRadiance.addLocal(radianceLi.mul(accuLiWeight));
		}

		///////////////////////////////////////////////////////////////////////////////
		// sample BSDF

		SurfaceSample surfaceSample;
		const BSDFcos* bsdfCos = hitSurfaceBehavior.getBsdfCos();
		bsdfCos->genImportanceSample(intersection, tracingRay, &surfaceSample);

		L = surfaceSample.m_direction;

		switch(surfaceSample.m_type)
		{
		case ESurfaceSampleType::REFLECTION:
		case ESurfaceSampleType::TRANSMISSION:
		{
			rayOriginDelta.set(surfaceSample.m_direction).mulLocal(rayDeltaDist);

			Vector3f liWeight = surfaceSample.m_LiWeight;

			if(numBounces >= 3)
			{
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
			}

			accuLiWeight.mulLocal(liWeight);
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

		// prepare for next iteration
		const Vector3f nextRayOrigin(intersection.getHitPosition().add(rayOriginDelta));
		const Vector3f nextRayDirection(L);
		tracingRay.setOrigin(nextRayOrigin);
		tracingRay.setDirection(nextRayDirection);
		numBounces++;
		intersection.clear();
	}// end while

	*out_radiance = accuRadiance;
}

}// end namespace ph