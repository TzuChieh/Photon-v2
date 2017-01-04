#include "Core/Integrator/BackwardMisIntegrator.h"
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
#include "Math/Math.h"
#include "Math/random_number.h"

#include <iostream>

#define RAY_DELTA_DIST 0.0001f
#define MAX_RAY_BOUNCES 10000

namespace ph
{

BackwardMisIntegrator::~BackwardMisIntegrator() = default;

void BackwardMisIntegrator::update(const World& world)
{
	// update nothing
}

void BackwardMisIntegrator::radianceAlongRay(const Ray& ray, const World& world, Vector3f* const out_radiance) const
{
	// common variables
	Vector3f rayOriginDelta;
	Vector3f accuRadiance(0, 0, 0);
	Vector3f accuLiWeight(1, 1, 1);
	Intersection intersection;
	SurfaceSample surfaceSample;

	// convenient variables
	const Intersector&       intersector  = world.getIntersector();
	const LightSampler&      lightSampler = world.getLightSampler();
	const PrimitiveMetadata* metadata     = nullptr;
	const BSDFcos*           bsdfCos      = nullptr;
	const Emitter*           emitter      = nullptr;

	// reversing the ray for backward tracing
	Ray tracingRay(ray.getOrigin(), ray.getDirection().mul(-1.0f), RAY_T_EPSILON, RAY_T_MAX);

	if(!intersector.isIntersecting(tracingRay, &intersection))
	{
		*out_radiance = Vector3f(0, 0, 0);
		return;
	}

	metadata = intersection.getHitPrimitive()->getMetadata();
	bsdfCos = metadata->surfaceBehavior.getBsdfCos();

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

		float32 emitterPickPdfW;
		emitter = lightSampler.pickEmitter(&emitterPickPdfW);
		if(emitter->isSurfaceEmissive())
		{
			Vector3f emittedRadiance;
			Vector3f emitPos;
			float32 directPdfW;
			emitter->genDirectSample(intersection.getHitPosition(), &emitPos, &emittedRadiance, &directPdfW);
			if(directPdfW > 0.0f)
			{
				Vector3f toLightVec = emitPos.sub(intersection.getHitPosition());
				Ray visRay(intersection.getHitPosition(), toLightVec.normalize(), RAY_DELTA_DIST, toLightVec.length() - RAY_DELTA_DIST * 2);
				Intersection visIntersection;
				if(!intersector.isIntersecting(visRay, &visIntersection))
				{
					Vector3f weight;
					bsdfCos->evaluate(intersection, toLightVec.normalize(), tracingRay.getDirection().mul(-1.0f), &weight);
					weight.mulLocal(accuLiWeight).divLocal(directPdfW * emitterPickPdfW);

					// avoid excessive, negative weight and possible NaNs
					rationalClamp(weight);

					accuRadiance.addLocal(emittedRadiance.mulLocal(weight).mulLocal(accuLiWeight));
				}
			}
		}

		///////////////////////////////////////////////////////////////////////////////
		// BSDF sample + indirect light sample

		bool keepSampling = true;

		surfaceSample.setImportanceSample(intersection, tracingRay.getDirection().mul(-1.0f));
		bsdfCos->genImportanceSample(surfaceSample);
		if(surfaceSample.liWeight.allZero())
		{
			break;
		}

		switch(surfaceSample.type)
		{
		case ESurfaceSampleType::REFLECTION:
		case ESurfaceSampleType::TRANSMISSION:
		{
			rayOriginDelta.set(surfaceSample.L).mulLocal(RAY_DELTA_DIST);

			Vector3f liWeight = surfaceSample.liWeight;

			if(numBounces >= 3)
			{
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
			rationalClamp(accuLiWeight);
		}
		break;

		default:
			std::cerr << "warning: unknown surface sample type in BackwardMisIntegrator detected" << std::endl;
			keepSampling = false;
			break;
		}// end switch surface sample type

		if(!keepSampling || accuLiWeight.squaredLength() == 0.0f)
		{
			break;
		}

		// prepare for next iteration

		const Vector3f nextRayOrigin(intersection.getHitPosition().add(rayOriginDelta));
		const Vector3f nextRayDirection(surfaceSample.L);
		tracingRay.setOrigin(nextRayOrigin);
		tracingRay.setDirection(nextRayDirection);
		
		intersection.clear();
		numBounces++;

		if(!intersector.isIntersecting(tracingRay, &intersection))
		{
			break;
		}

		metadata = intersection.getHitPrimitive()->getMetadata();
		bsdfCos = metadata->surfaceBehavior.getBsdfCos();
	}

	*out_radiance = accuRadiance;
}

// NaNs will be clamped to 0
void BackwardMisIntegrator::rationalClamp(Vector3f& value)
{
	value.x = value.x > 0.0f && value.x < 10000.0f ? value.x : 0.0f;
	value.y = value.y > 0.0f && value.y < 10000.0f ? value.y : 0.0f;
	value.z = value.z > 0.0f && value.z < 10000.0f ? value.z : 0.0f;
}

// power heuristic with beta = 2
float32 BackwardMisIntegrator::misWeight(float32 pdf1W, float32 pdf2W)
{
	pdf1W *= pdf1W;
	pdf2W *= pdf2W;
	return pdf1W / (pdf1W + pdf2W);
}

}// end namespace ph