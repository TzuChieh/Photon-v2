#include "Core/Integrator/BackwardMisIntegrator.h"
#include "Core/Ray.h"
#include "World/World.h"
#include "World/Intersector.h"
#include "World/LightSampler/LightSampler.h"
#include "Math/Vector3f.h"
#include "Core/Intersection.h"
#include "Core/Sample/SurfaceSample.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/BSDFcos.h"
#include "Core/Primitive/Primitive.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Math/Math.h"
#include "Math/random_number.h"
#include "Core/Sample/DirectLightSample.h"

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
	Vector3f V;
	Intersection intersection;
	SurfaceSample surfaceSample;
	DirectLightSample directLightSample;

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

	V = tracingRay.getDirection().mul(-1.0f);

	// sidedness agreement between real geometry and shading (phong-interpolated) normal
	if(intersection.getHitSmoothNormal().dot(V) * intersection.getHitGeoNormal().dot(V) <= 0.0f)
	{
		*out_radiance = Vector3f(0, 0, 0);
		return;
	}

	metadata = intersection.getHitPrimitive()->getMetadata();
	bsdfCos = metadata->surfaceBehavior.getBsdfCos();

	if(metadata->surfaceBehavior.getEmitter())
	{
		Vector3f radianceLe;
		metadata->surfaceBehavior.getEmitter()->evalEmittedRadiance(intersection, &radianceLe);
		accuRadiance.addLocal(radianceLe);
	}

	for(uint32 numBounces = 0; numBounces < MAX_RAY_BOUNCES; numBounces++)
	{
		///////////////////////////////////////////////////////////////////////////////
		// direct light sample

		directLightSample.setDirectSample(intersection.getHitPosition());
		lightSampler.genDirectSample(directLightSample);
		if(directLightSample.isDirectSampleGood())
		{
			const Vector3f toLightVec = directLightSample.emitPos.sub(directLightSample.targetPos);

			// sidedness agreement between real geometry and shading (phong-interpolated) normal
			if(!(intersection.getHitSmoothNormal().dot(toLightVec) * intersection.getHitGeoNormal().dot(toLightVec) <= 0.0f))
			{
				const Ray visRay(intersection.getHitPosition(), toLightVec.normalize(), RAY_DELTA_DIST, toLightVec.length() - RAY_DELTA_DIST * 2);
				if(!intersector.isIntersecting(visRay))
				{
					Vector3f weight;
					surfaceSample.setEvaluation(intersection, visRay.getDirection(), V);
					bsdfCos->evaluate(surfaceSample);
					if(surfaceSample.isEvaluationGood())
					{
						const float32 bsdfCosPdfW = bsdfCos->calcImportanceSamplePdfW(surfaceSample);
						const float32 misWeighting = misWeight(directLightSample.pdfW, bsdfCosPdfW);
						//const float32 misWeighting = 0.0f;

						weight = surfaceSample.liWeight;
						weight.mulLocal(accuLiWeight).mulLocal(misWeighting / directLightSample.pdfW);

						// avoid excessive, negative weight and possible NaNs
						rationalClamp(weight);

						accuRadiance.addLocal(directLightSample.radianceLe.mul(weight));
					}
				}
			}
		}// end direct light sample

		///////////////////////////////////////////////////////////////////////////////
		// BSDF sample + indirect light sample
		
		surfaceSample.setImportanceSample(intersection, tracingRay.getDirection().mul(-1.0f));
		bsdfCos->genImportanceSample(surfaceSample);
		// blackness check & sidedness agreement between real geometry and shading (phong-interpolated) normal
		if(!surfaceSample.isImportanceSampleGood() ||
		   intersection.getHitSmoothNormal().dot(surfaceSample.L) * intersection.getHitGeoNormal().dot(surfaceSample.L) <= 0.0f)
		{
			break;
		}

		const float32 bsdfCosPdfW = bsdfCos->calcImportanceSamplePdfW(surfaceSample);
		const Vector3f directLitPos = intersection.getHitPosition();

		// trace a ray via BSDFcos's suggestion
		rayOriginDelta.set(surfaceSample.L).mulLocal(RAY_DELTA_DIST);
		tracingRay.setOrigin(intersection.getHitPosition().add(rayOriginDelta));
		tracingRay.setDirection(surfaceSample.L);
		intersection.clear();
		if(!intersector.isIntersecting(tracingRay, &intersection))
		{
			break;
		}

		V = tracingRay.getDirection().mul(-1.0f);
		// sidedness agreement between real geometry and shading (phong-interpolated) normal
		if(intersection.getHitSmoothNormal().dot(V) * intersection.getHitGeoNormal().dot(V) <= 0.0f)
		{
			break;
		}

		metadata = intersection.getHitPrimitive()->getMetadata();
		bsdfCos = metadata->surfaceBehavior.getBsdfCos();
		emitter = metadata->surfaceBehavior.getEmitter();
		if(emitter)
		{
			Vector3f radianceLe;
			metadata->surfaceBehavior.getEmitter()->evalEmittedRadiance(intersection, &radianceLe);

			// TODO: how to handle delta BSDF distributions?

			const float32 directLightPdfW = lightSampler.calcDirectPdfW(directLitPos, 
				intersection.getHitPosition(), 
				intersection.getHitSmoothNormal(), 
				emitter, intersection.getHitPrimitive());
			const float32 misWeighting = misWeight(bsdfCosPdfW, directLightPdfW);
			//const float32 misWeighting = 1.0f;

			Vector3f weight = surfaceSample.liWeight;
			weight.mulLocal(accuLiWeight).mulLocal(misWeighting);

			// avoid excessive, negative weight and possible NaNs
			rationalClamp(weight);

			accuRadiance.addLocal(radianceLe.mulLocal(weight));
		}

		Vector3f liWeight = surfaceSample.liWeight;

		if(numBounces >= 3)
		{
			const float32 rrSurviveRate = Math::clamp(liWeight.avg(), 0.0001f, 1.0f);
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
				break;
			}
		}

		accuLiWeight.mulLocal(liWeight);

		// avoid excessive, negative weight and possible NaNs
		rationalClamp(accuLiWeight);

		if(accuLiWeight.allZero())
		{
			break;
		}

		numBounces++;
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