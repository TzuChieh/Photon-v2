#include "Core/Integrator/BackwardPathIntegrator.h"
#include "Core/Ray.h"
#include "World/World.h"
#include "World/Intersector.h"
#include "World/LightSampler/LightSampler.h"
#include "Math/TVector3.h"
#include "Core/Intersection.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/BSDFcos.h"
#include "Core/Sample/SurfaceSample.h"
#include "Math/Math.h"
#include "Math/Color.h"
#include "Math/random_number.h"
#include "Core/Primitive/Primitive.h"
#include "Core/Emitter/Emitter.h"
#include "FileIO/InputPacket.h"

#include <iostream>

#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 1

namespace ph
{

BackwardPathIntegrator::BackwardPathIntegrator(const InputPacket& packet) : 
	Integrator(packet)
{

}

BackwardPathIntegrator::~BackwardPathIntegrator() = default;

void BackwardPathIntegrator::update(const World& world)
{
	// update nothing
}

void BackwardPathIntegrator::radianceAlongRay(const Sample& sample, const World& world, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const
{
	const real rayDeltaDist = 0.0001_r;
	const Intersector& intersector = world.getIntersector();

	uint32 numBounces = 0;
	Vector3R accuRadiance(0, 0, 0);
	Vector3R accuLiWeight(1, 1, 1);
	Vector3R rayOriginDelta;
	Intersection intersection;

	Ray ray;
	camera.genSensingRay(sample, &ray);

	// backward tracing to light
	Ray tracingRay(ray.getOrigin(), ray.getDirection().mul(-1.0f), RAY_T_EPSILON, RAY_T_MAX);

	const Primitive* lastPrimitive = nullptr;

	while(numBounces <= MAX_RAY_BOUNCES && intersector.isIntersecting(tracingRay, &intersection))
	{
		bool keepSampling = true;

		const auto* const metadata = intersection.getHitPrimitive()->getMetadata();
		const SurfaceBehavior& hitSurfaceBehavior = metadata->surfaceBehavior;
		const Vector3R V = tracingRay.getDirection().mul(-1.0f);

		///////////////////////////////////////////////////////////////////////////////
		// sample emitted radiance

		// sidedness agreement between real geometry and shading (phong-interpolated) normal
		if(intersection.getHitSmoothNormal().dot(V) * intersection.getHitGeoNormal().dot(V) <= 0.0f)
		{
			break;
		}

		if(hitSurfaceBehavior.getEmitter())
		{
			Vector3R radianceLi;
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
		surfaceSample.setImportanceSample(intersection, tracingRay.getDirection().mul(-1.0f));
		hitSurfaceBehavior.getBsdfCos()->genImportanceSample(surfaceSample);

		// blackness check & sidedness agreement between real geometry and shading (phong-interpolated) normal
		if(surfaceSample.liWeight.isZero() ||
		   intersection.getHitSmoothNormal().dot(surfaceSample.L) * intersection.getHitGeoNormal().dot(surfaceSample.L) <= 0.0f)
		{
			break;
		}

		switch(surfaceSample.type)
		{
		case ESurfaceSampleType::REFLECTION:
		case ESurfaceSampleType::TRANSMISSION:
		{
			rayOriginDelta.set(surfaceSample.L).mulLocal(rayDeltaDist);

			Vector3R liWeight = surfaceSample.liWeight;

			if(numBounces >= 3)
			{
				//const real rrSurviveRate = liWeight.clamp(0.0f, 1.0f).max();
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
		const Vector3R nextRayOrigin(intersection.getHitPosition().add(rayOriginDelta));
		const Vector3R nextRayDirection(surfaceSample.L);
		tracingRay.setOrigin(nextRayOrigin);
		tracingRay.setDirection(nextRayDirection);
		numBounces++;
		intersection.clear();
	}// end while

	out_senseEvents.push_back(SenseEvent(sample.m_cameraX, sample.m_cameraY, accuRadiance));
}

}// end namespace ph