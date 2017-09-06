#include "Core/Integrator/LightTracingIntegrator.h"
#include "Core/Ray.h"
#include "World/Scene.h"
#include "Math/TVector3.h"
#include "Core/Intersection.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/Math.h"
#include "Math/Random.h"
#include "Core/Sample/DirectLightSample.h"
#include "Core/Camera/Camera.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"
#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Math/TVector2.h"
#include "Core/Quantity/SpectralStrength.h"

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

void LightTracingIntegrator::radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const
{
	//const Scene&  scene  = *data.scene;
	//const Camera& camera = *data.camera;

	//uint32 numBounces = 0;

	//real emitterPickPdf;
	//const Emitter* emitter = scene.pickEmitter(&emitterPickPdf);
	//if(!emitter || emitterPickPdf <= 0)
	//{
	//	return;
	//}

	//Ray emitterRay;
	//SpectralStrength emitterRadianceLe;
	//Vector3R eN;
	//real emitterPdfA;
	//real emitterPdfW;
	//emitter->genSensingRay(&emitterRay, &emitterRadianceLe, &eN, &emitterPdfA, &emitterPdfW);

	//emitterPdfA *= emitterPickPdf;
	//if(emitterRadianceLe.isZero() || emitterPdfA <= 0 || emitterPdfW <= 0)
	//{
	//	return;
	//}

	//// 0 bounce
	//{
	//	const Vector3R toCameraVec(camera.getPosition().sub(emitterRay.getOrigin()));
	//	const Ray toCameraRay(emitterRay.getOrigin(), toCameraVec.normalize(), 
	//		RAY_DELTA_DIST, toCameraVec.length() - 2 * RAY_DELTA_DIST);
	//	if(!scene.isIntersecting(toCameraRay))
	//	{
	//		Vector3R cameraImportanceWe;
	//		Vector2R filmCoord;
	//		real cameraPdfW;
	//		real cameraPdfA = 1;// always pinhole for now
	//		real filmArea;
	//		camera.evalEmittedImportanceAndPdfW(emitterRay.getOrigin(), &filmCoord, 
	//			&cameraImportanceWe, &filmArea, &cameraPdfW);
	//		if(cameraPdfW > 0)
	//		{
	//			SpectralStrength weight(1);
	//			weight.mulLocal(cameraImportanceWe.div(cameraPdfA));
	//			weight.divLocal(emitterPdfA);
	//			const real G = eN.absDot(toCameraRay.getDirection()) *
	//				camera.getDirection().absDot(toCameraRay.getDirection()) / toCameraVec.lengthSquared();
	//			weight.mulLocal(G);

	//			rationalClamp(weight);

	//			// assuming same amount of radiance is emitted in every direction
	//			out_senseEvents.push_back(SenseEvent(/*filmCoord.x, filmCoord.y, */emitterRadianceLe.mul(weight)));
	//		}
	//	}
	//}

	//Intersection intersection;
	//Vector3R throughput(1, 1, 1);
	//while(numBounces < MAX_RAY_BOUNCES && scene.isIntersecting(emitterRay, &intersection))
	//{
	//	const PrimitiveMetadata* const metadata = intersection.getHitPrimitive()->getMetadata();
	//	const BSDF* const bsdf = metadata->surfaceBehavior.getBsdf();
	//	const Vector3R V(emitterRay.getDirection().mul(-1.0f));
	//	
	//	if(intersection.getHitGeoNormal().dot(V) * intersection.getHitSmoothNormal().dot(V) <= 0.0f)
	//	{
	//		return;
	//	}

	//	// try connecting to camera
	//	{
	//		const Vector3R toCameraVec(camera.getPosition().sub(intersection.getHitPosition()));
	//		if(!(intersection.getHitGeoNormal().dot(toCameraVec) * intersection.getHitSmoothNormal().dot(toCameraVec) <= 0.0f))
	//		{
	//			const Ray toCameraRay(intersection.getHitPosition(), toCameraVec.normalize(), 
	//				RAY_DELTA_DIST, toCameraVec.length() - 2 * RAY_DELTA_DIST);
	//			if(!scene.isIntersecting(toCameraRay))
	//			{
	//				BsdfEvaluation bsdfEval;
	//				bsdfEval.inputs.set(intersection, toCameraRay.getDirection(), V);
	//				bsdf->evaluate(bsdfEval);
	//				if(bsdfEval.outputs.isGood())
	//				{
	//					Vector3R cameraImportanceWe;
	//					Vector2R filmCoord;
	//					real cameraPdfW;
	//					real cameraPdfA = 1.0_r;// always pinhole for now
	//					real filmArea;
	//					camera.evalEmittedImportanceAndPdfW(intersection.getHitPosition(), &filmCoord, 
	//						&cameraImportanceWe, &filmArea, &cameraPdfW);
	//					if(cameraPdfW > 0.0_r)
	//					{
	//						Vector3R weight(1.0_r, 1.0_r, 1.0_r);
	//						weight.mulLocal(cameraImportanceWe.div(cameraPdfA));
	//						weight.mulLocal(bsdfEval.outputs.bsdf);
	//						weight.mulLocal(1.0_r / (emitterPdfA * emitterPdfW));
	//						weight.mulLocal(throughput);
	//						const real G = intersection.getHitSmoothNormal().absDot(toCameraRay.getDirection()) *
	//							camera.getDirection().absDot(toCameraRay.getDirection()) / toCameraVec.lengthSquared();
	//						weight.mulLocal(G);

	//						rationalClamp(weight);

	//						out_senseEvents.push_back(SenseEvent(/*filmCoord.x, filmCoord.y, */emitterRadianceLe.mul(weight)));
	//					}
	//				}
	//			}
	//		}
	//	}

	//	BsdfSample bsdfSample;
	//	bsdfSample.inputs.set(intersection, V);
	//	bsdf->sample(bsdfSample);

	//	if(!bsdfSample.outputs.isGood())
	//	{
	//		return;
	//	}

	//	const Vector3R& N = intersection.getHitSmoothNormal();
	//	const Vector3R& L = bsdfSample.outputs.L;

	//	if(intersection.getHitGeoNormal().dot(L) * intersection.getHitSmoothNormal().dot(L) <= 0.0_r)
	//	{
	//		return;
	//	}

	//	Vector3R liWeight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));

	//	if(numBounces >= 3)
	//	{
	//		const real rrSurviveRate = Math::clamp(liWeight.avg(), 0.0001_r, 1.0_r);
	//		const real rrSpin = Random::genUniformReal_i0_e1();

	//		// russian roulette >> survive
	//		if(rrSurviveRate > rrSpin)
	//		{
	//			const real rrScale = 1.0_r / rrSurviveRate;
	//			liWeight.mulLocal(rrScale);
	//		}
	//		// russian roulette >> dead
	//		else
	//		{
	//			break;
	//		}
	//	}

	//	throughput.mulLocal(liWeight);
	//	rationalClamp(throughput);
	//	if(throughput.isZero())
	//	{
	//		return;
	//	}

	//	// prepare for next intersection

	//	emitterRay.setOrigin(intersection.getHitPosition());
	//	emitterRay.setDirection(L);
	//	numBounces++;
	//}
}

// NaNs will be clamped to 0
void LightTracingIntegrator::rationalClamp(Vector3R& value)
{
	value.x = value.x > 0.0f && value.x < 10000.0f ? value.x : 0.0f;
	value.y = value.y > 0.0f && value.y < 10000.0f ? value.y : 0.0f;
	value.z = value.z > 0.0f && value.z < 10000.0f ? value.z : 0.0f;
}

// command interface

LightTracingIntegrator::LightTracingIntegrator(const InputPacket& packet) :
	Integrator(packet)
{

}

SdlTypeInfo LightTracingIntegrator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "light-tracing");
}

void LightTracingIntegrator::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<LightTracingIntegrator>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<LightTracingIntegrator> LightTracingIntegrator::ciLoad(const InputPacket& packet)
{
	return std::make_unique<LightTracingIntegrator>(packet);
}

}// end namespace ph