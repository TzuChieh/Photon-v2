#include "Core/Integrator/BackwardLightIntegrator.h"
#include "Core/Ray.h"
#include "World/World.h"
#include "World/Intersector.h"
#include "World/LightSampler/LightSampler.h"
#include "Math/Vector3f.h"
#include "Core/Intersection.h"
#include "Core/SurfaceBehavior/SurfaceSample.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/BSDFcos.h"
#include "Core/Primitive/Primitive.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/ESurfaceSampleType.h"
#include "Math/Math.h"
#include "Math/random_number.h"

#include <iostream>

#define MAX_RAY_BOUNCES 10000

namespace ph
{

BackwardLightIntegrator::~BackwardLightIntegrator() = default;

void BackwardLightIntegrator::update(const World& world)
{
	// update nothing
}

void BackwardLightIntegrator::radianceAlongRay(const Ray& ray, const World& world, Vector3f* const out_radiance) const
{
	const float32 rayDeltaDist = 0.0001f;
	Vector3f rayOriginDelta;

	const Intersector& intersector = world.getIntersector();
	const LightSampler& lightSampler = world.getLightSampler();

	// reverse ray for backward tracing
	Ray tracingRay(ray.getOrigin(), ray.getDirection().mul(-1.0f), RAY_T_EPSILON, RAY_T_MAX);
	Intersection intersection;
	Vector3f accuRadiance(0, 0, 0);
	Vector3f accuLiWeight(1, 1, 1);

	if(!intersector.isIntersecting(tracingRay, &intersection))
	{
		*out_radiance = Vector3f(0, 0, 0);
		return;
	}

	const PrimitiveMetadata* metadata = intersection.getHitPrimitive()->getMetadata();
	SurfaceSample surfaceSample;
	const BSDFcos* bsdfCos = metadata->surfaceBehavior.getBsdfCos();

	if(metadata->surfaceBehavior.getEmitter())
	{
		Vector3f radianceLi;
		metadata->surfaceBehavior.getEmitter()->evalEmittedRadiance(intersection, &radianceLi);
		accuRadiance.addLocal(radianceLi);
	}

	for(uint32 numBounces = 0; numBounces < MAX_RAY_BOUNCES; numBounces++)
	{
		///////////////////////////////////////////////////////////////////////////////
		// direct light sample

		float32 emitterPickPDF;
		const Emitter* emitter = lightSampler.pickEmitter(&emitterPickPDF);
		if(emitter->isSurfaceEmissive())
		{
			Vector3f emittedRadiance;
			Vector3f emitPos;
			float32 directPdf;
			emitter->genDirectSample(intersection.getHitPosition(), &emitPos, &emittedRadiance, &directPdf);
			if(directPdf > 0.0f)
			{
				Vector3f toLightVec = emitPos.sub(intersection.getHitPosition());
				Ray visRay(intersection.getHitPosition(), toLightVec.normalize(), 0.0001f, toLightVec.length() - 0.0001f * 2);
				Intersection visIntersection;
				if(!intersector.isIntersecting(visRay, &visIntersection))
				{
					Vector3f weight;
					bsdfCos->evaluate(intersection, toLightVec.normalize(), tracingRay.getDirection().mul(-1.0f), &weight);
					weight.divLocal(directPdf * emitterPickPDF);

					// avoid excessive, negative weight and possible NaNs
					weight.x = weight.x > 0.0f && weight.x < 10000.0f ? weight.x : 0.0f;
					weight.y = weight.y > 0.0f && weight.y < 10000.0f ? weight.y : 0.0f;
					weight.z = weight.z > 0.0f && weight.z < 10000.0f ? weight.z : 0.0f;

					accuRadiance.addLocal(emittedRadiance.mulLocal(weight).mulLocal(accuLiWeight));
				}
			}
		}

		///////////////////////////////////////////////////////////////////////////////
		// indirect light sample

		bool keepSampling = true;

		bsdfCos->genImportanceSample(intersection, tracingRay, &surfaceSample);
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

			// avoid excessive, negative weight and possible NaNs
			accuLiWeight.x = accuLiWeight.x > 0.0f && accuLiWeight.x < 10000.0f ? accuLiWeight.x : 0.0f;
			accuLiWeight.y = accuLiWeight.y > 0.0f && accuLiWeight.y < 10000.0f ? accuLiWeight.y : 0.0f;
			accuLiWeight.z = accuLiWeight.z > 0.0f && accuLiWeight.z < 10000.0f ? accuLiWeight.z : 0.0f;
			if(accuLiWeight.squaredLength() == 0.0f)
			{
				keepSampling = false;
			}
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
		const Vector3f nextRayDirection(surfaceSample.m_direction);
		tracingRay.setOrigin(nextRayOrigin);
		tracingRay.setDirection(nextRayDirection);
		numBounces++;
		intersection.clear();

		if(!intersector.isIntersecting(tracingRay, &intersection))
		{
			break;
		}

		metadata = intersection.getHitPrimitive()->getMetadata();
		bsdfCos = metadata->surfaceBehavior.getBsdfCos();
	}

	*out_radiance = accuRadiance;
}

}// end namespace ph