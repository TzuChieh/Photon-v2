#include "Core/Integrator/BackwardLightIntegrator.h"
#include "Core/Ray.h"
#include "World/Scene.h"
#include "Math/TVector3.h"
#include "Core/Intersection.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/BSDF.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/Math.h"
#include "Math/Random.h"
#include "Core/Sample/DirectLightSample.h"
#include "FileIO/InputPacket.h"
#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"
#include "Core/Quantity/SpectralStrength.h"

#include <iostream>

#define RAY_DELTA_DIST 0.0001f
#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 1

namespace ph
{

BackwardLightIntegrator::~BackwardLightIntegrator() = default;

void BackwardLightIntegrator::update(const Scene& scene)
{
	// update nothing
}

void BackwardLightIntegrator::radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const
{
	const Scene&  scene  = *data.scene;
	const Camera& camera = *data.camera;

	/*Ray ray;
	camera.genSensingRay(sample, &ray);*/

	// common variables
	Vector3R rayOriginDelta;
	SpectralStrength accuRadiance(0);
	SpectralStrength accuLiWeight(1);
	Vector3R V;
	Intersection intersection;
	BsdfSample     bsdfSample;
	BsdfEvaluation bsdfEval;
	DirectLightSample directLightSample;

	// convenient variables
	const PrimitiveMetadata* metadata = nullptr;
	const BSDF*              bsdf     = nullptr;

	// reversing the ray for backward tracing
	Ray tracingRay(ray.getOrigin(), ray.getDirection().mul(-1.0f), 0.0001_r, Ray::MAX_T, ray.getTime());// HACK: hard-coded number

	if(!scene.isIntersecting(tracingRay, &intersection))
	{
		out_senseEvents.push_back(SenseEvent(/*sample.m_cameraX, sample.m_cameraY, */accuRadiance));
		return;
	}

	V = tracingRay.getDirection().mul(-1.0f);

	// sidedness agreement between real geometry and shading (phong-interpolated) normal
	if(intersection.getHitSmoothNormal().dot(V) * intersection.getHitGeoNormal().dot(V) <= 0.0_r)
	{
		out_senseEvents.push_back(SenseEvent(/*sample.m_cameraX, sample.m_cameraY, */accuRadiance));
		return;
	}

	metadata = intersection.getHitPrimitive()->getMetadata();
	bsdf     = metadata->surfaceBehavior.getBsdf();

	if(metadata->surfaceBehavior.getEmitter())
	{
		SpectralStrength radianceLe;
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
			if(!(intersection.getHitSmoothNormal().dot(toLightVec) * intersection.getHitGeoNormal().dot(toLightVec) <= 0.0_r))
			{
				const Ray visRay(intersection.getHitPosition(), toLightVec.normalize(), RAY_DELTA_DIST, toLightVec.length() - RAY_DELTA_DIST * 2, ray.getTime());
				if(!scene.isIntersecting(visRay))
				{
					SpectralStrength weight;
					bsdfEval.inputs.set(intersection, visRay.getDirection(), V);
					bsdf->evaluate(bsdfEval);
					if(bsdfEval.outputs.isGood())
					{
						const Vector3R& N = intersection.getHitSmoothNormal();
						const Vector3R& L = visRay.getDirection();

						weight = bsdfEval.outputs.bsdf.mul(N.absDot(L));
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

		bsdfSample.inputs.set(intersection, tracingRay.getDirection().mul(-1.0_r));
		bsdf->sample(bsdfSample);

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
			rayOriginDelta.set(L).mulLocal(RAY_DELTA_DIST);

			SpectralStrength liWeight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));

			if(numBounces >= 3)
			{
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
		const Vector3R nextRayDirection(L);
		tracingRay.setOrigin(nextRayOrigin);
		tracingRay.setDirection(nextRayDirection);

		if(!scene.isIntersecting(tracingRay, &intersection))
		{
			break;
		}

		V = tracingRay.getDirection().mul(-1.0_r);

		// sidedness agreement between real geometry and shading (phong-interpolated) normal
		if(intersection.getHitSmoothNormal().dot(V) * intersection.getHitGeoNormal().dot(V) <= 0.0_r)
		{
			break;
		}

		metadata = intersection.getHitPrimitive()->getMetadata();
		bsdf     = metadata->surfaceBehavior.getBsdf();
	}

	out_senseEvents.push_back(SenseEvent(/*sample.m_cameraX, sample.m_cameraY, */accuRadiance));
}

void BackwardLightIntegrator::rationalClamp(SpectralStrength& value)
{
	// TODO: should negative value be allowed?
	value.clampLocal(0.0f, 10000.0f);
}

// command interface

BackwardLightIntegrator::BackwardLightIntegrator(const InputPacket& packet) :
	Integrator(packet)
{

}

SdlTypeInfo BackwardLightIntegrator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "backward-light");
}

std::unique_ptr<BackwardLightIntegrator> BackwardLightIntegrator::ciLoad(const InputPacket& packet)
{
	return std::make_unique<BackwardLightIntegrator>(packet);
}

ExitStatus BackwardLightIntegrator::ciExecute(const std::shared_ptr<BackwardLightIntegrator>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph