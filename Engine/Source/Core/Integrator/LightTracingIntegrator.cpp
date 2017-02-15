#include "Core/Integrator/LightTracingIntegrator.h"
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
#include "Camera/Camera.h"

#include <iostream>

#define RAY_DELTA_DIST 0.0001f
#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 5

namespace ph
{

LightTracingIntegrator::~LightTracingIntegrator() = default;

void LightTracingIntegrator::update(const Scene& scene)
{
	// update nothing
}

void LightTracingIntegrator::radianceAlongRay(const Sample& sample, const Scene& scene, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const
{
	uint32 numBounces = 0;
	
	real emitterPickPdf;
	const Emitter* emitter = scene.pickEmitter(&emitterPickPdf);
	if(!emitter || emitterPickPdf <= 0)
	{
		return;
	}

	Ray emitterRay;
	Vector3R emitterRadianceLe;
	Vector3R eN;
	real emitterPdfA;
	real emitterPdfW;
	emitter->genSensingRay(&emitterRay, &emitterRadianceLe, &eN, &emitterPdfA, &emitterPdfW);
	emitterPdfA *= emitterPickPdf;
	if(emitterRadianceLe.isZero() || emitterPdfA <= 0 || emitterPdfW <= 0)
	{
		return;
	}

	// 0 bounce
	{
		const Vector3R toCameraVec(camera.getPosition().sub(emitterRay.getOrigin()));
		const Ray toCameraRay(emitterRay.getOrigin(), toCameraVec.normalize(), 
			RAY_DELTA_DIST, toCameraVec.length() - 2 * RAY_DELTA_DIST);
		if(!scene.isIntersecting(toCameraRay))
		{
			Vector3R cameraImportanceWe;
			Vector2f filmCoord;
			real cameraPdfW;
			real cameraPdfA = 1;// always pinhole for now
			real filmArea;
			camera.evalEmittedImportanceAndPdfW(emitterRay.getOrigin(), &filmCoord, 
				&cameraImportanceWe, &filmArea, &cameraPdfW);
			if(cameraPdfW > 0)
			{
				Vector3R weight(1, 1, 1);
				weight.mulLocal(cameraImportanceWe.div(cameraPdfA));
				weight.divLocal(emitterPdfA);
				const real G = eN.absDot(toCameraRay.getDirection()) *
					camera.getDirection().absDot(toCameraRay.getDirection()) / toCameraVec.squaredLength();
				weight.mulLocal(G);

				rationalClamp(weight);

				// assuming same amount of radiance is emitted in every direction
				out_senseEvents.push_back(SenseEvent(filmCoord.x, filmCoord.y, emitterRadianceLe.mul(weight)));
			}
		}
	}

	Intersection intersection;
	Vector3R throughput(1, 1, 1);
	while(numBounces < MAX_RAY_BOUNCES && scene.isIntersecting(emitterRay, &intersection))
	{
		const PrimitiveMetadata* const metadata = intersection.getHitPrimitive()->getMetadata();
		const BSDFcos* const bsdfCos = metadata->surfaceBehavior.getBsdfCos();
		const Vector3R V(emitterRay.getDirection().mul(-1.0f));
		
		if(intersection.getHitGeoNormal().dot(V) * intersection.getHitSmoothNormal().dot(V) <= 0.0f)
		{
			return;
		}

		// try connecting to camera
		{
			const Vector3R toCameraVec(camera.getPosition().sub(intersection.getHitPosition()));
			if(!(intersection.getHitGeoNormal().dot(toCameraVec) * intersection.getHitSmoothNormal().dot(toCameraVec) <= 0.0f))
			{
				const Ray toCameraRay(intersection.getHitPosition(), toCameraVec.normalize(), 
					RAY_DELTA_DIST, toCameraVec.length() - 2 * RAY_DELTA_DIST);
				if(!scene.isIntersecting(toCameraRay))
				{
					SurfaceSample surfaceSample;
					surfaceSample.setEvaluation(intersection, toCameraRay.getDirection(), V);
					bsdfCos->evaluate(surfaceSample);
					if(surfaceSample.isEvaluationGood())
					{
						Vector3R cameraImportanceWe;
						Vector2f filmCoord;
						real cameraPdfW;
						real cameraPdfA = 1.0_r;// always pinhole for now
						real filmArea;
						camera.evalEmittedImportanceAndPdfW(intersection.getHitPosition(), &filmCoord, 
							&cameraImportanceWe, &filmArea, &cameraPdfW);
						if(cameraPdfW > 0.0_r)
						{
							Vector3R weight(1.0_r, 1.0_r, 1.0_r);
							weight.mulLocal(cameraImportanceWe.div(cameraPdfA));
							weight.mulLocal(surfaceSample.liWeight);
							weight.mulLocal(1.0_r / (emitterPdfA * emitterPdfW));
							weight.mulLocal(throughput);
							const real G = intersection.getHitSmoothNormal().absDot(toCameraRay.getDirection()) *
								camera.getDirection().absDot(toCameraRay.getDirection()) / toCameraVec.squaredLength();
							weight.mulLocal(G);

							weight.divLocal(toCameraRay.getDirection().absDot(intersection.getHitSmoothNormal()));

							rationalClamp(weight);

							out_senseEvents.push_back(SenseEvent(filmCoord.x, filmCoord.y, emitterRadianceLe.mul(weight)));
						}
					}
				}
			}
		}

		SurfaceSample surfaceSample;
		surfaceSample.setImportanceSample(intersection, V);
		bsdfCos->genImportanceSample(surfaceSample);
		if(!surfaceSample.isImportanceSampleGood())
		{
			return;
		}

		if(intersection.getHitGeoNormal().dot(surfaceSample.L) * intersection.getHitSmoothNormal().dot(surfaceSample.L) <= 0.0_r)
		{
			return;
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

		throughput.mulLocal(liWeight);
		rationalClamp(throughput);
		if(throughput.isZero())
		{
			return;
		}

		// prepare for next intersection

		emitterRay.setOrigin(intersection.getHitPosition());
		emitterRay.setDirection(surfaceSample.L);
		intersection.clear();
		numBounces++;
	}
}

// NaNs will be clamped to 0
void LightTracingIntegrator::rationalClamp(Vector3R& value)
{
	value.x = value.x > 0.0f && value.x < 10000.0f ? value.x : 0.0f;
	value.y = value.y > 0.0f && value.y < 10000.0f ? value.y : 0.0f;
	value.z = value.z > 0.0f && value.z < 10000.0f ? value.z : 0.0f;
}

}// end namespace ph