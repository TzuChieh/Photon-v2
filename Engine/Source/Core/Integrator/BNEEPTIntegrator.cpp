#include "Core/Integrator/BNEEPTIntegrator.h"
#include "Core/Ray.h"
#include "World/Scene.h"
#include "Math/TVector3.h"
#include "Core/HitDetail.h"
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

BNEEPTIntegrator::~BNEEPTIntegrator() = default;

void BNEEPTIntegrator::update(const Scene& scene)
{
	// update nothing
}

void BNEEPTIntegrator::radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const
{
	const Scene&  scene  = *data.scene;
	const Camera& camera = *data.camera;

	// common variables
	//
	SpectralStrength accuRadiance(0);
	SpectralStrength accuLiWeight(1);
	HitProbe         hitProbe;
	SurfaceHit       surfaceHit;
	Vector3R         V;

	// reversing the ray for backward tracing
	//
	Ray tracingRay = Ray(ray).reverse();
	tracingRay.setMinT(0.0001_r);// HACK: hard-coded number
	tracingRay.setMaxT(std::numeric_limits<real>::max());

	if(!scene.isIntersecting(tracingRay, &hitProbe))
	{
		out_senseEvents.push_back(SenseEvent(accuRadiance));
		return;
	}

	// 0-bounce direct lighting
	//
	{
		surfaceHit = SurfaceHit(tracingRay, hitProbe);

		// sidedness agreement between real geometry and shading normal
		//
		V = tracingRay.getDirection().mul(-1.0_r);
		if(surfaceHit.getGeometryNormal().dot(V) * surfaceHit.getShadingNormal().dot(V) <= 0.0_r)
		{
			out_senseEvents.push_back(SenseEvent(accuRadiance));
			return;
		}

		const PrimitiveMetadata* metadata        = surfaceHit.getDetail().getPrimitive()->getMetadata();
		const SurfaceBehavior&   surfaceBehavior = metadata->surfaceBehavior;
		if(surfaceBehavior.getEmitter())
		{
			SpectralStrength radianceLi;
			surfaceBehavior.getEmitter()->evalEmittedRadiance(surfaceHit, &radianceLi);
			accuRadiance.addLocal(radianceLi);
		}
	}

	// ray bouncing around the scene (1 ~ N bounces)
	//
	for(uint32 numBounces = 0; numBounces < MAX_RAY_BOUNCES; numBounces++)
	{
		// direct light sample
		//
		DirectLightSample directLightSample;
		directLightSample.setDirectSample(surfaceHit.getPosition());
		scene.genDirectSample(directLightSample);
		if(directLightSample.isDirectSampleGood())
		{
			const Vector3R& toLightVec = directLightSample.emitPos.sub(directLightSample.targetPos);

			// sidedness agreement between real geometry and shading  normal
			//
			if(surfaceHit.getGeometryNormal().dot(toLightVec) * surfaceHit.getShadingNormal().dot(toLightVec) > 0.0_r)
			{
				const Ray visRay(surfaceHit.getPosition(), toLightVec.normalize(), RAY_DELTA_DIST, toLightVec.length() - RAY_DELTA_DIST * 2, ray.getTime());
				if(!scene.isIntersecting(visRay))
				{
					const PrimitiveMetadata* metadata        = surfaceHit.getDetail().getPrimitive()->getMetadata();
					const SurfaceBehavior&   surfaceBehavior = metadata->surfaceBehavior;

					BsdfEvaluation bsdfEval;
					bsdfEval.inputs.set(surfaceHit, visRay.getDirection(), V);
					surfaceBehavior.getSurfaceOptics()->evalBsdf(bsdfEval);
					if(bsdfEval.outputs.isGood())
					{
						BsdfPdfQuery bsdfPdfQuery;
						bsdfPdfQuery.inputs.set(bsdfEval);
						surfaceBehavior.getSurfaceOptics()->calcBsdfSamplePdf(bsdfPdfQuery);

						const real      bsdfSamplePdfW = bsdfPdfQuery.outputs.sampleDirPdfW;
						const real      misWeighting   = misWeight(directLightSample.pdfW, bsdfSamplePdfW);
						const Vector3R& N = surfaceHit.getShadingNormal();
						const Vector3R& L = visRay.getDirection();

						SpectralStrength weight;
						weight = bsdfEval.outputs.bsdf.mul(N.absDot(L));
						weight.mulLocal(accuLiWeight).mulLocal(misWeighting / directLightSample.pdfW);

						// avoid excessive, negative weight and possible NaNs
						//
						rationalClamp(weight);

						accuRadiance.addLocal(directLightSample.radianceLe.mul(weight));
					}
				}
			}
		}// end direct light sample

		// BSDF sample + indirect light sample
		//
		{
			const PrimitiveMetadata* metadata        = surfaceHit.getDetail().getPrimitive()->getMetadata();
			const SurfaceBehavior*   surfaceBehavior = &(metadata->surfaceBehavior);

			BsdfSample bsdfSample;
			bsdfSample.inputs.set(surfaceHit, tracingRay.getDirection().mul(-1));
			surfaceBehavior->getSurfaceOptics()->genBsdfSample(bsdfSample);

			const Vector3R& N = surfaceHit.getShadingNormal();
			const Vector3R& L = bsdfSample.outputs.L;

			// blackness check & sidedness agreement between real geometry and shading normal
			//
			if(!bsdfSample.outputs.isGood() ||
			   surfaceHit.getGeometryNormal().dot(L) * surfaceHit.getShadingNormal().dot(L) <= 0.0_r)
			{
				break;
			}

			BsdfPdfQuery bsdfPdfQuery;
			bsdfPdfQuery.inputs.set(bsdfSample);
			surfaceBehavior->getSurfaceOptics()->calcBsdfSamplePdf(bsdfPdfQuery);

			const real      bsdfSamplePdfW = bsdfPdfQuery.outputs.sampleDirPdfW;
			const Vector3R& directLitPos   = surfaceHit.getPosition();

			// trace the ray via BSDF's suggestion
			//
			tracingRay.setOrigin(surfaceHit.getPosition());
			tracingRay.setDirection(L);
			if(!scene.isIntersecting(tracingRay, &hitProbe))
			{
				break;
			}
			surfaceHit = SurfaceHit(tracingRay, hitProbe);

			metadata        = surfaceHit.getDetail().getPrimitive()->getMetadata();
			surfaceBehavior = &(metadata->surfaceBehavior);

			const Emitter* emitter = metadata->surfaceBehavior.getEmitter();
			if(emitter)
			{
				SpectralStrength radianceLe;
				metadata->surfaceBehavior.getEmitter()->evalEmittedRadiance(surfaceHit, &radianceLe);

				const real directLightPdfW = scene.calcDirectPdfW(
					directLitPos,
					surfaceHit.getPosition(),
					surfaceHit.getShadingNormal(),
					emitter, 
					surfaceHit.getDetail().getPrimitive());
				const real misWeighting = misWeight(bsdfSamplePdfW, directLightPdfW);

				SpectralStrength weight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));
				weight.mulLocal(accuLiWeight).mulLocal(misWeighting);

				// avoid excessive, negative weight and possible NaNs
				//
				rationalClamp(weight);

				accuRadiance.addLocal(radianceLe.mulLocal(weight));
			}

			SpectralStrength currentLiWeight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));
			if(numBounces >= 3)
			{
				const real rrSurviveRate = Math::clamp(currentLiWeight.calcLuminance(), 0.0001_r, 1.0_r);
				const real rrSpin = Random::genUniformReal_i0_e1();

				// russian roulette >> survive
				if(rrSurviveRate > rrSpin)
				{
					const real rrScale = 1.0_r / rrSurviveRate;
					currentLiWeight.mulLocal(rrScale);
				}
				// russian roulette >> dead
				else
				{
					break;
				}
			}
			accuLiWeight.mulLocal(currentLiWeight);

			// avoid excessive, negative weight and possible NaNs
			//
			rationalClamp(accuLiWeight);

			if(accuLiWeight.isZero())
			{
				break;
			}

			V = tracingRay.getDirection().mul(-1.0_r);
		}
	}// end for each bounces

	out_senseEvents.push_back(SenseEvent(accuRadiance));
}

void BNEEPTIntegrator::rationalClamp(SpectralStrength& value)
{
	// TODO: should negative value be allowed?
	value.clampLocal(0.0_r, 10000.0_r);
}

// power heuristic with beta = 2
real BNEEPTIntegrator::misWeight(real pdf1W, real pdf2W)
{
	pdf1W *= pdf1W;
	pdf2W *= pdf2W;
	return pdf1W / (pdf1W + pdf2W);
}

// command interface

BNEEPTIntegrator::BNEEPTIntegrator(const InputPacket& packet) :
	Integrator(packet)
{}

SdlTypeInfo BNEEPTIntegrator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "bneept");
}

void BNEEPTIntegrator::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<BNEEPTIntegrator>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<BNEEPTIntegrator> BNEEPTIntegrator::ciLoad(const InputPacket& packet)
{
	return std::make_unique<BNEEPTIntegrator>(packet);
}

}// end namespace ph