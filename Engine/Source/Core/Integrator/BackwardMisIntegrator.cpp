#include "Core/Integrator/BackwardMisIntegrator.h"
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

#include <iostream>

#define RAY_DELTA_DIST 0.0001f
#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 1

namespace ph
{

BackwardMisIntegrator::BackwardMisIntegrator(const InputPacket& packet) : 
	Integrator(packet)
{

}

BackwardMisIntegrator::~BackwardMisIntegrator() = default;

void BackwardMisIntegrator::update(const Scene& scene)
{
	// update nothing
}

void BackwardMisIntegrator::radianceAlongRay(const Sample& sample, const Scene& scene, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const
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
	const Emitter*           emitter      = nullptr;

	// reversing the ray for backward tracing
	Ray tracingRay(ray.getOrigin(), ray.getDirection().mul(-1), 0.0001_r, Ray::MAX_T);// HACK: hard-coded number

	if(!scene.isIntersecting(tracingRay, &intersection))
	{
		out_senseEvents.push_back(SenseEvent(sample.m_cameraX, sample.m_cameraY, accuRadiance));
		return;
	}

	V = tracingRay.getDirection().mul(-1);

	// sidedness agreement between real geometry and shading (phong-interpolated) normal
	if(intersection.getHitSmoothNormal().dot(V) * intersection.getHitGeoNormal().dot(V) <= 0)
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
			const Vector3R toLightVec = directLightSample.emitPos.sub(directLightSample.targetPos);

			// sidedness agreement between real geometry and shading (phong-interpolated) normal
			if(!(intersection.getHitSmoothNormal().dot(toLightVec) * intersection.getHitGeoNormal().dot(toLightVec) <= 0))
			{
				const Ray visRay(intersection.getHitPosition(), toLightVec.normalize(), RAY_DELTA_DIST, toLightVec.length() - RAY_DELTA_DIST * 2);
				if(!scene.isIntersecting(visRay))
				{
					Vector3R weight;
					surfaceSample.setEvaluation(intersection, visRay.getDirection(), V);
					bsdfCos->evaluate(surfaceSample);
					if(surfaceSample.isEvaluationGood())
					{
						const real bsdfCosPdfW = bsdfCos->calcImportanceSamplePdfW(surfaceSample);
						const real misWeighting = misWeight(directLightSample.pdfW, bsdfCosPdfW);

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
		
		surfaceSample.setImportanceSample(intersection, tracingRay.getDirection().mul(-1));
		bsdfCos->genImportanceSample(surfaceSample);
		// blackness check & sidedness agreement between real geometry and shading (phong-interpolated) normal
		if(!surfaceSample.isImportanceSampleGood() ||
		   intersection.getHitSmoothNormal().dot(surfaceSample.L) * intersection.getHitGeoNormal().dot(surfaceSample.L) <= 0)
		{
			break;
		}

		const real bsdfCosPdfW = bsdfCos->calcImportanceSamplePdfW(surfaceSample);
		const Vector3R directLitPos = intersection.getHitPosition();

		// trace a ray via BSDFcos's suggestion
		rayOriginDelta.set(surfaceSample.L).mulLocal(RAY_DELTA_DIST);
		tracingRay.setOrigin(intersection.getHitPosition().add(rayOriginDelta));
		tracingRay.setDirection(surfaceSample.L);
		intersection.clear();
		if(!scene.isIntersecting(tracingRay, &intersection))
		{
			break;
		}

		V = tracingRay.getDirection().mul(-1.0_r);
		// sidedness agreement between real geometry and shading (phong-interpolated) normal
		if(intersection.getHitSmoothNormal().dot(V) * intersection.getHitGeoNormal().dot(V) <= 0)
		{
			break;
		}

		metadata = intersection.getHitPrimitive()->getMetadata();
		bsdfCos = metadata->surfaceBehavior.getBsdfCos();
		emitter = metadata->surfaceBehavior.getEmitter();
		if(emitter)
		{
			Vector3R radianceLe;
			metadata->surfaceBehavior.getEmitter()->evalEmittedRadiance(intersection, &radianceLe);

			// TODO: how to handle delta BSDF distributions?

			const real directLightPdfW = scene.calcDirectPdfW(directLitPos,
				intersection.getHitPosition(), 
				intersection.getHitSmoothNormal(), 
				emitter, intersection.getHitPrimitive());
			const real misWeighting = misWeight(bsdfCosPdfW, directLightPdfW);

			Vector3R weight = surfaceSample.liWeight;
			weight.mulLocal(accuLiWeight).mulLocal(misWeighting);

			// avoid excessive, negative weight and possible NaNs
			rationalClamp(weight);

			accuRadiance.addLocal(radianceLe.mulLocal(weight));
		}

		Vector3R liWeight = surfaceSample.liWeight;

		if(numBounces >= 3)
		{
			const real rrSurviveRate = Math::clamp(liWeight.avg(), 0.0001_r, 1.0_r);
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
				break;
			}
		}

		accuLiWeight.mulLocal(liWeight);

		// avoid excessive, negative weight and possible NaNs
		rationalClamp(accuLiWeight);

		if(accuLiWeight.isZero())
		{
			break;
		}
	}

	out_senseEvents.push_back(SenseEvent(sample.m_cameraX, sample.m_cameraY, accuRadiance));
}

// NaNs will be clamped to 0
void BackwardMisIntegrator::rationalClamp(Vector3R& value)
{
	value.x = value.x > 0.0_r && value.x < 10000.0_r ? value.x : 0.0_r;
	value.y = value.y > 0.0_r && value.y < 10000.0_r ? value.y : 0.0_r;
	value.z = value.z > 0.0_r && value.z < 10000.0_r ? value.z : 0.0_r;
}

// power heuristic with beta = 2
real BackwardMisIntegrator::misWeight(real pdf1W, real pdf2W)
{
	pdf1W *= pdf1W;
	pdf2W *= pdf2W;
	return pdf1W / (pdf1W + pdf2W);
}

}// end namespace ph