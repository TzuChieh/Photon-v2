#include "Core/Integrator/LightTracingIntegrator.h"
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
#include "Camera/Camera.h"

#include <iostream>

#define RAY_DELTA_DIST 0.0001f
#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 5

namespace ph
{

LightTracingIntegrator::~LightTracingIntegrator() = default;

void LightTracingIntegrator::update(const World& world)
{
	// update nothing
}

void LightTracingIntegrator::radianceAlongRay(const Sample& sample, const World& world, const Camera& camera, std::vector<SenseEvent>& out_senseEvents) const
{
	float32 numBounces = 0;

	// convenient variables
	const Intersector&  intersector  = world.getIntersector();
	const LightSampler& lightSampler = world.getLightSampler();
	
	float32 emitterPickPdf;
	const Emitter* emitter = lightSampler.pickEmitter(&emitterPickPdf);
	if(!emitter || emitterPickPdf <= 0.0f)
	{
		return;
	}

	Ray emitterRay;
	Vector3f emitterRadianceLe;
	Vector3f eN;
	float32 emitterPdfA;
	float32 emitterPdfW;
	emitter->genSensingRay(&emitterRay, &emitterRadianceLe, &eN, &emitterPdfA, &emitterPdfW);
	emitterPdfA *= emitterPickPdf;
	if(emitterRadianceLe.allZero() || emitterPdfA <= 0.0f || emitterPdfW <= 0.0f)
	{
		return;
	}

	// 0 bounce
	{
		const Vector3f toCameraVec(camera.getPosition().sub(emitterRay.getOrigin()));
		const Ray toCameraRay(emitterRay.getOrigin(), toCameraVec.normalize(), 
			RAY_DELTA_DIST, toCameraVec.length() - 2.0f * RAY_DELTA_DIST);
		if(!intersector.isIntersecting(toCameraRay))
		{
			Vector3f cameraImportanceWe;
			Vector2f filmCoord;
			float32 cameraPdfW;
			float32 cameraPdfA = 1.0f;// always pinhole for now
			float32 filmArea;
			camera.evalEmittedImportanceAndPdfW(emitterRay.getOrigin(), &filmCoord, 
				&cameraImportanceWe, &filmArea, &cameraPdfW);
			if(cameraPdfW > 0.0f)
			{
				Vector3f weight(1, 1, 1);
				weight.mulLocal(cameraImportanceWe.div(cameraPdfA));
				weight.divLocal(emitterPdfA);
				const float32 G = eN.absDot(toCameraRay.getDirection()) * 
					camera.getDirection().absDot(toCameraRay.getDirection()) / toCameraVec.squaredLength();
				weight.mulLocal(G);

				rationalClamp(weight);

				// assuming same amount of radiance is emitted in every direction
				out_senseEvents.push_back(SenseEvent(filmCoord.x, filmCoord.y, emitterRadianceLe.mul(weight)));
			}
		}
	}

	Intersection intersection;
	Vector3f throughput(1, 1, 1);
	while(numBounces < MAX_RAY_BOUNCES && intersector.isIntersecting(emitterRay, &intersection))
	{
		const PrimitiveMetadata* const metadata = intersection.getHitPrimitive()->getMetadata();
		const BSDFcos* const bsdfCos = metadata->surfaceBehavior.getBsdfCos();
		const Vector3f V(emitterRay.getDirection().mul(-1.0f));
		
		if(intersection.getHitGeoNormal().dot(V) * intersection.getHitSmoothNormal().dot(V) <= 0.0f)
		{
			return;
		}

		// try connecting to camera
		{
			const Vector3f toCameraVec(camera.getPosition().sub(intersection.getHitPosition()));
			if(!(intersection.getHitGeoNormal().dot(toCameraVec) * intersection.getHitSmoothNormal().dot(toCameraVec) <= 0.0f))
			{
				const Ray toCameraRay(intersection.getHitPosition(), toCameraVec.normalize(), 
					RAY_DELTA_DIST, toCameraVec.length() - 2.0f * RAY_DELTA_DIST);
				if(!intersector.isIntersecting(toCameraRay))
				{
					SurfaceSample surfaceSample;
					surfaceSample.setEvaluation(intersection, toCameraRay.getDirection(), V);
					bsdfCos->evaluate(surfaceSample);
					if(surfaceSample.isEvaluationGood())
					{
						Vector3f cameraImportanceWe;
						Vector2f filmCoord;
						float32 cameraPdfW;
						float32 cameraPdfA = 1.0f;// always pinhole for now
						float32 filmArea;
						camera.evalEmittedImportanceAndPdfW(intersection.getHitPosition(), &filmCoord, 
							&cameraImportanceWe, &filmArea, &cameraPdfW);
						if(cameraPdfW > 0.0f)
						{
							Vector3f weight(1, 1, 1);
							weight.mulLocal(cameraImportanceWe.div(cameraPdfA));
							weight.mulLocal(surfaceSample.liWeight);
							weight.mulLocal(1.0f / (emitterPdfA * emitterPdfW));
							weight.mulLocal(throughput);
							const float32 G = intersection.getHitSmoothNormal().absDot(toCameraRay.getDirection()) *
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

		if(intersection.getHitGeoNormal().dot(surfaceSample.L) * intersection.getHitSmoothNormal().dot(surfaceSample.L) <= 0.0f)
		{
			return;
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

		throughput.mulLocal(liWeight);
		rationalClamp(throughput);
		if(throughput.allZero())
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
void LightTracingIntegrator::rationalClamp(Vector3f& value)
{
	value.x = value.x > 0.0f && value.x < 10000.0f ? value.x : 0.0f;
	value.y = value.y > 0.0f && value.y < 10000.0f ? value.y : 0.0f;
	value.z = value.z > 0.0f && value.z < 10000.0f ? value.z : 0.0f;
}

}// end namespace ph