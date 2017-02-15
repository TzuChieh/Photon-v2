#include "Core/Integrator/BackwardLightIntegrator.h"
#include "Core/Ray.h"
#include "World/Scene.h"
#include "Math/TVector3.h"
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
#include "FileIO/InputPacket.h"

#include <iostream>

#define RAY_DELTA_DIST 0.0001f
#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 1

namespace ph
{

BackwardLightIntegrator::BackwardLightIntegrator(const InputPacket& packet) : 
	Integrator(packet)
{

}

BackwardLightIntegrator::~BackwardLightIntegrator() = default;

void BackwardLightIntegrator::update(const Scene& scene)
{
	// update nothing
}

void BackwardLightIntegrator::radianceAlongRay(const Sample& sample, const Scene& scene, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const
{
	Ray ray;
	camera.genSensingRay(sample, &ray);

	// common variables
	Vector3R rayOriginDelta;
	Vector3R accuRadiance(0, 0, 0);
	Vector3R accuLiWeight(1, 1, 1);
	Vector3R V;
	Intersection intersection;
	SurfaceSample surfaceSample;
	DirectLightSample directLightSample;

	// convenient variables
	const PrimitiveMetadata* metadata     = nullptr;
	const BSDFcos*           bsdfCos      = nullptr;

	// reversing the ray for backward tracing
	Ray tracingRay(ray.getOrigin(), ray.getDirection().mul(-1.0f), 0.0001_r, Ray::MAX_T);// HACK: hard-coded number

	if(!scene.isIntersecting(tracingRay, &intersection))
	{
		out_senseEvents.push_back(SenseEvent(sample.m_cameraX, sample.m_cameraY, accuRadiance));
		return;
	}

	V = tracingRay.getDirection().mul(-1.0f);

	// sidedness agreement between real geometry and shading (phong-interpolated) normal
	if(intersection.getHitSmoothNormal().dot(V) * intersection.getHitGeoNormal().dot(V) <= 0.0f)
	{
		out_senseEvents.push_back(SenseEvent(sample.m_cameraX, sample.m_cameraY, accuRadiance));
		return;
	}

	metadata = intersection.getHitPrimitive()->getMetadata();
	bsdfCos = metadata->surfaceBehavior.getBsdfCos();

	if(metadata->surfaceBehavior.getEmitter())
	{
		Vector3R radianceLe;
		metadata->surfaceBehavior.getEmitter()->evalEmittedRadiance(intersection, &radianceLe);
		accuRadiance.addLocal(radianceLe);
	}

	for(uint32 numBounces = 0; numBounces < MAX_RAY_BOUNCES; numBounces++)
	{
		///////////////////////////////////////////////////////////////////////////////
		// direct light sample

		directLightSample.setDirectSample(intersection.getHitPosition());
		scene.genDirectSample(directLightSample);
		if(directLightSample.isDirectSampleGood())
		{
			const Vector3R toLightVec = directLightSample.emitPos.sub(intersection.getHitPosition());

			// sidedness agreement between real geometry and shading (phong-interpolated) normal
			if(!(intersection.getHitSmoothNormal().dot(toLightVec) * intersection.getHitGeoNormal().dot(toLightVec) <= 0.0f))
			{
				const Ray visRay(intersection.getHitPosition(), toLightVec.normalize(), RAY_DELTA_DIST, toLightVec.length() - RAY_DELTA_DIST * 2);
				if(!scene.isIntersecting(visRay))
				{
					Vector3R weight;
					surfaceSample.setEvaluation(intersection, visRay.getDirection(), V);
					bsdfCos->evaluate(surfaceSample);
					if(surfaceSample.isEvaluationGood())
					{
						weight = surfaceSample.liWeight;
						weight.mulLocal(accuLiWeight).divLocal(directLightSample.pdfW);

						// avoid excessive, negative weight and possible NaNs
						rationalClamp(weight);

						accuRadiance.addLocal(directLightSample.radianceLe.mul(weight));
					}
				}
			}
		}// end direct light sample

		///////////////////////////////////////////////////////////////////////////////
		// BSDF sample + indirect light sample

		bool keepSampling = true;

		surfaceSample.setImportanceSample(intersection, tracingRay.getDirection().mul(-1.0f));
		bsdfCos->genImportanceSample(surfaceSample);
		// blackness check & sidedness agreement between real geometry and shading (phong-interpolated) normal
		if(!surfaceSample.isImportanceSampleGood() ||
		   intersection.getHitSmoothNormal().dot(surfaceSample.L) * intersection.getHitGeoNormal().dot(surfaceSample.L) <= 0.0f)
		{
			break;
		}

		switch(surfaceSample.type)
		{
		case ESurfaceSampleType::REFLECTION:
		case ESurfaceSampleType::TRANSMISSION:
		{
			rayOriginDelta.set(surfaceSample.L).mulLocal(RAY_DELTA_DIST);

			Vector3R liWeight = surfaceSample.liWeight;

			if(numBounces >= 3)
			{
				const real rrSurviveRate = Math::clamp(liWeight.avg(), 0.0001_r, 1.0_r);
				//const real rrSurviveRate = Math::clamp(Color::linearRgbLuminance(liWeight), 0.0001f, 1.0f);
				const real rrSpin = genRandomReal_0_1_uniform();

				// russian roulette >> survive
				if(rrSurviveRate > rrSpin)
				{
					const real rrScale = 1.0_r / rrSurviveRate;
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

		if(!keepSampling || accuLiWeight.isZero())
		{
			break;
		}

		// prepare for next iteration

		const Vector3R nextRayOrigin(intersection.getHitPosition().add(rayOriginDelta));
		const Vector3R nextRayDirection(surfaceSample.L);
		tracingRay.setOrigin(nextRayOrigin);
		tracingRay.setDirection(nextRayDirection);
		intersection.clear();

		if(!scene.isIntersecting(tracingRay, &intersection))
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
	}

	out_senseEvents.push_back(SenseEvent(sample.m_cameraX, sample.m_cameraY, accuRadiance));
}

// NaNs will be clamped to 0
void BackwardLightIntegrator::rationalClamp(Vector3R& value)
{
	value.x = value.x > 0.0f && value.x < 10000.0f ? value.x : 0.0f;
	value.y = value.y > 0.0f && value.y < 10000.0f ? value.y : 0.0f;
	value.z = value.z > 0.0f && value.z < 10000.0f ? value.z : 0.0f;
}

}// end namespace ph