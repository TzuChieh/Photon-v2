#include "Core/Integrator/BackwardMisIntegrator.h"
#include "Core/Ray.h"
#include "World/Scene.h"
#include "Math/TVector3.h"
#include "Core/IntersectionDetail.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/Math.h"
#include "Math/Random.h"
#include "Core/Sample/DirectLightSample.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"
#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/Quantity/SpectralStrength.h"

#include <iostream>

#define RAY_DELTA_DIST 0.0001f
#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 1

namespace ph
{

BackwardMisIntegrator::~BackwardMisIntegrator() = default;

void BackwardMisIntegrator::update(const Scene& scene)
{
	// update nothing
}

void BackwardMisIntegrator::radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const
{
	//const Scene&  scene  = *data.scene;
	//const Camera& camera = *data.camera;

	///*Ray ray;
	//camera.genSensingRay(sample, &ray);*/

	//// common variables
	//Vector3R rayOriginDelta;
	//SpectralStrength accuRadiance(0);
	//SpectralStrength accuLiWeight(1);
	//Vector3R V;
	//Intersection intersection;
	//BsdfEvaluation bsdfEval;
	//BsdfSample     bsdfSample;
	//BsdfPdfQuery   bsdfPdfQuery;
	//DirectLightSample directLightSample;

	//// convenient variables
	//const PrimitiveMetadata* metadata = nullptr;
	//const BSDF*              bsdf     = nullptr;
	//const Emitter*           emitter  = nullptr;

	//// reversing the ray for backward tracing
	//Ray tracingRay(ray.getOrigin(), ray.getDirection().mul(-1), 0.0001_r, Ray::MAX_T, ray.getTime());// HACK: hard-coded number

	//if(!scene.isIntersecting(tracingRay, &intersection))
	//{
	//	out_senseEvents.push_back(SenseEvent(/*sample.m_cameraX, sample.m_cameraY, */accuRadiance));
	//	return;
	//}

	//V = tracingRay.getDirection().mul(-1);

	//// sidedness agreement between real geometry and shading (phong-interpolated) normal
	//if(intersection.getHitSmoothNormal().dot(V) * intersection.getHitGeoNormal().dot(V) <= 0)
	//{
	//	out_senseEvents.push_back(SenseEvent(/*sample.m_cameraX, sample.m_cameraY, */accuRadiance));
	//	return;
	//}

	//metadata = intersection.getHitPrimitive()->getMetadata();
	//bsdf     = metadata->surfaceBehavior.getBsdf();

	//if(metadata->surfaceBehavior.getEmitter())
	//{
	//	SpectralStrength radianceLe;
	//	metadata->surfaceBehavior.getEmitter()->evalEmittedRadiance(intersection, &radianceLe);
	//	accuRadiance.addLocal(radianceLe);
	//}

	//for(uint32 numBounces = 0; numBounces < MAX_RAY_BOUNCES; numBounces++)
	//{
	//	///////////////////////////////////////////////////////////////////////////////
	//	// direct light sample

	//	directLightSample.setDirectSample(intersection.getHitPosition());
	//	scene.genDirectSample(directLightSample);
	//	if(directLightSample.isDirectSampleGood())
	//	{
	//		const Vector3R toLightVec = directLightSample.emitPos.sub(directLightSample.targetPos);

	//		// sidedness agreement between real geometry and shading (phong-interpolated) normal
	//		if(!(intersection.getHitSmoothNormal().dot(toLightVec) * intersection.getHitGeoNormal().dot(toLightVec) <= 0))
	//		{
	//			const Ray visRay(intersection.getHitPosition(), toLightVec.normalize(), RAY_DELTA_DIST, toLightVec.length() - RAY_DELTA_DIST * 2, ray.getTime());
	//			if(!scene.isIntersecting(visRay))
	//			{
	//				bsdfEval.inputs.set(intersection, visRay.getDirection(), V);
	//				bsdf->evaluate(bsdfEval);
	//				if(bsdfEval.outputs.isGood())
	//				{
	//					bsdfPdfQuery.inputs.set(bsdfEval);
	//					bsdf->calcPdf(bsdfPdfQuery);
	//					const real bsdfSamplePdfW = bsdfPdfQuery.outputs.sampleDirPdfW;
	//					const real misWeighting = misWeight(directLightSample.pdfW, bsdfSamplePdfW);

	//					const Vector3R& N = intersection.getHitSmoothNormal();
	//					const Vector3R& L = visRay.getDirection();

	//					SpectralStrength weight;
	//					weight = bsdfEval.outputs.bsdf.mul(N.absDot(L));
	//					weight.mulLocal(accuLiWeight).mulLocal(misWeighting / directLightSample.pdfW);

	//					// avoid excessive, negative weight and possible NaNs
	//					rationalClamp(weight);

	//					accuRadiance.addLocal(directLightSample.radianceLe.mul(weight));
	//				}
	//			}
	//		}
	//	}// end direct light sample

	//	///////////////////////////////////////////////////////////////////////////////
	//	// BSDF sample + indirect light sample

	//	bsdfSample.inputs.set(intersection, tracingRay.getDirection().mul(-1));
	//	bsdf->sample(bsdfSample);

	//	const Vector3R N = intersection.getHitSmoothNormal();
	//	const Vector3R L = bsdfSample.outputs.L;

	//	// blackness check & sidedness agreement between real geometry and shading (phong-interpolated) normal
	//	if(!bsdfSample.outputs.isGood() ||
	//	   intersection.getHitSmoothNormal().dot(L) * intersection.getHitGeoNormal().dot(L) <= 0)
	//	{
	//		break;
	//	}

	//	bsdfPdfQuery.inputs.set(bsdfSample);
	//	bsdf->calcPdf(bsdfPdfQuery);
	//	const real bsdfSamplePdfW = bsdfPdfQuery.outputs.sampleDirPdfW;
	//	const Vector3R directLitPos = intersection.getHitPosition();

	//	// trace a ray via BSDF's suggestion
	//	rayOriginDelta.set(L).mulLocal(RAY_DELTA_DIST);
	//	tracingRay.setOrigin(intersection.getHitPosition().add(rayOriginDelta));
	//	tracingRay.setDirection(L);
	//	if(!scene.isIntersecting(tracingRay, &intersection))
	//	{
	//		break;
	//	}

	//	V = tracingRay.getDirection().mul(-1.0_r);
	//	// sidedness agreement between real geometry and shading (phong-interpolated) normal
	//	if(intersection.getHitSmoothNormal().dot(V) * intersection.getHitGeoNormal().dot(V) <= 0)
	//	{
	//		break;
	//	}

	//	metadata = intersection.getHitPrimitive()->getMetadata();
	//	bsdf     = metadata->surfaceBehavior.getBsdf();
	//	emitter  = metadata->surfaceBehavior.getEmitter();
	//	if(emitter)
	//	{
	//		SpectralStrength radianceLe;
	//		metadata->surfaceBehavior.getEmitter()->evalEmittedRadiance(intersection, &radianceLe);

	//		// TODO: handle delta BSDF distributions

	//		const real directLightPdfW = scene.calcDirectPdfW(directLitPos,
	//			intersection.getHitPosition(), 
	//			intersection.getHitSmoothNormal(), 
	//			emitter, intersection.getHitPrimitive());
	//		const real misWeighting = misWeight(bsdfSamplePdfW, directLightPdfW);

	//		SpectralStrength weight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));
	//		weight.mulLocal(accuLiWeight).mulLocal(misWeighting);

	//		// avoid excessive, negative weight and possible NaNs
	//		rationalClamp(weight);

	//		accuRadiance.addLocal(radianceLe.mulLocal(weight));
	//	}

	//	SpectralStrength liWeight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));

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

	//	accuLiWeight.mulLocal(liWeight);

	//	// avoid excessive, negative weight and possible NaNs
	//	rationalClamp(accuLiWeight);

	//	if(accuLiWeight.isZero())
	//	{
	//		break;
	//	}
	//}

	//out_senseEvents.push_back(SenseEvent(/*sample.m_cameraX, sample.m_cameraY, */accuRadiance));
}

void BackwardMisIntegrator::rationalClamp(SpectralStrength& value)
{
	// TODO: should negative value be allowed?
	value.clampLocal(0.0_r, 10000.0_r);
}

// power heuristic with beta = 2
real BackwardMisIntegrator::misWeight(real pdf1W, real pdf2W)
{
	pdf1W *= pdf1W;
	pdf2W *= pdf2W;
	return pdf1W / (pdf1W + pdf2W);
}

// command interface

BackwardMisIntegrator::BackwardMisIntegrator(const InputPacket& packet) :
	Integrator(packet)
{

}

SdlTypeInfo BackwardMisIntegrator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "backward-mis");
}

void BackwardMisIntegrator::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<BackwardMisIntegrator>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<BackwardMisIntegrator> BackwardMisIntegrator::ciLoad(const InputPacket& packet)
{
	return std::make_unique<BackwardMisIntegrator>(packet);
}

}// end namespace ph