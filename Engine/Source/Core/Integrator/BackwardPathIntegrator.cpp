#include "Core/Integrator/BackwardPathIntegrator.h"
#include "Core/Ray.h"
#include "World/Scene.h"
#include "Math/TVector3.h"
#include "Core/Intersection.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/BSDF.h"
#include "Math/Math.h"
#include "Math/Color.h"
#include "Math/Random.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Emitter/Emitter.h"
#include "FileIO/InputPacket.h"
#include "Core/SurfaceBehavior/BsdfSample.h"

#include <iostream>

#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 1

namespace ph
{

BackwardPathIntegrator::~BackwardPathIntegrator() = default;

void BackwardPathIntegrator::update(const Scene& scene)
{
	// update nothing
}

void BackwardPathIntegrator::radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const
{
	const Scene&  scene  = *data.scene;
	const Camera& camera = *data.camera;

	const real rayDeltaDist = 0.0001_r;

	uint32 numBounces = 0;
	Vector3R accuRadiance(0, 0, 0);
	Vector3R accuLiWeight(1, 1, 1);
	Vector3R rayOriginDelta;
	Intersection intersection;

	/*Ray ray;
	camera.genSensingRay(sample, &ray);*/

	// backward tracing to light
	Ray tracingRay(ray.getOrigin(), ray.getDirection().mul(-1.0f), 0.0001_r, Ray::MAX_T, ray.getTime());// HACK: hard-coded number

	const Primitive* lastPrimitive = nullptr;

	while(numBounces <= MAX_RAY_BOUNCES && scene.isIntersecting(tracingRay, &intersection))
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
			if(accuLiWeight.lengthSquared() == 0.0f)
			{
				break;
			}

			accuRadiance.addLocal(radianceLi.mul(accuLiWeight));
		}

		///////////////////////////////////////////////////////////////////////////////
		// sample BSDF

		BsdfSample bsdfSample;
		bsdfSample.inputs.set(intersection, tracingRay.getDirection().mul(-1.0f));
		hitSurfaceBehavior.getBsdf()->sample(bsdfSample);

		const Vector3R N = intersection.getHitSmoothNormal();
		const Vector3R L = bsdfSample.outputs.L;

		// blackness check & sidedness agreement between real geometry and shading (phong-interpolated) normal
		if(!bsdfSample.outputs.isGood() ||
		   intersection.getHitSmoothNormal().dot(L) * intersection.getHitGeoNormal().dot(L) <= 0.0_r)
		{
			break;
		}

		switch(bsdfSample.outputs.phenomenon)
		{
		case ESurfacePhenomenon::REFLECTION:
		case ESurfacePhenomenon::TRANSMISSION:
		{
			rayOriginDelta.set(L).mulLocal(rayDeltaDist);

			Vector3R liWeight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));

			if(numBounces >= 3)
			{
				//const real rrSurviveRate = liWeight.clamp(0.0f, 1.0f).max();
				const real rrSurviveRate = Math::clamp(liWeight.avg(), 0.0001_r, 1.0_r);
				//const real rrSurviveRate = Math::clamp(Color::linearRgbLuminance(liWeight), 0.0001f, 1.0f);
				const real rrSpin = Random::genUniformReal_i0_e1();

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
			std::cerr << "warning: unknown surface phenomenon type in BackwardPathIntegrator detected" << std::endl;
			keepSampling = false;
			break;
		}// end switch surface sample type

		if(!keepSampling)
		{
			break;
		}

		// prepare for next iteration
		const Vector3R nextRayOrigin(intersection.getHitPosition().add(rayOriginDelta));
		const Vector3R nextRayDirection(L);
		tracingRay.setOrigin(nextRayOrigin);
		tracingRay.setDirection(nextRayDirection);
		numBounces++;
	}// end while

	out_senseEvents.push_back(SenseEvent(/*sample.m_cameraX, sample.m_cameraY, */accuRadiance));
}

// command interface

BackwardPathIntegrator::BackwardPathIntegrator(const InputPacket& packet) :
	Integrator(packet)
{

}

SdlTypeInfo BackwardPathIntegrator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "backward-path");
}

std::unique_ptr<BackwardPathIntegrator> BackwardPathIntegrator::ciLoad(const InputPacket& packet)
{
	return std::make_unique<BackwardPathIntegrator>(packet);
}

ExitStatus BackwardPathIntegrator::ciExecute(const std::shared_ptr<BackwardPathIntegrator>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph