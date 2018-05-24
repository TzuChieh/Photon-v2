#include "Core/Integrator/BVPTIntegrator.h"
#include "Core/Ray.h"
#include "World/Scene.h"
#include "Math/TVector3.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Math/Math.h"
#include "Math/Color.h"
#include "Math/Random.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Emitter/Emitter.h"
#include "FileIO/SDL/InputPacket.h"
#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Integrator/Utility/PtVolumetricEstimator.h"
#include "Core/Integrator/Utility/TSurfaceEventDispatcher.h"

#include <iostream>

#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 1

namespace ph
{

BVPTIntegrator::~BVPTIntegrator() = default;

void BVPTIntegrator::update(const Scene& scene)
{
	// update nothing
}

void BVPTIntegrator::radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const
{
	const Scene&  scene  = *data.scene;
	const Camera& camera = *data.camera;
	
	//const real rayDeltaDist = 0.00001_r;

	uint32 numBounces = 0;
	SpectralStrength accuRadiance(0);
	SpectralStrength accuLiWeight(1);
	//Vector3R rayOriginDelta;
	HitProbe hitProbe;

	/*Ray ray;
	camera.genSensingRay(sample, &ray);*/

	// backward tracing to light
	Ray tracingRay = Ray(ray).reverse();
	tracingRay.setMinT(0.0001_r);// HACK: hard-coded number
	tracingRay.setMaxT(std::numeric_limits<real>::max());

	while(numBounces <= MAX_RAY_BOUNCES && scene.isIntersecting(tracingRay, &hitProbe))
	{
		bool keepSampling = true;

		const SurfaceHit surfaceHit(tracingRay, hitProbe);
		const HitDetail& hitDetail = surfaceHit.getDetail();

		const auto* const metadata = hitDetail.getPrimitive()->getMetadata();
		const SurfaceBehavior& hitSurfaceBehavior = metadata->getSurface();
		const Vector3R& V = tracingRay.getDirection().mul(-1.0f);

		///////////////////////////////////////////////////////////////////////////////
		// sample emitted radiance

		// sidedness agreement between real geometry and shading (phong-interpolated) normal
		if(hitDetail.getShadingNormal().dot(V) * hitDetail.getGeometryNormal().dot(V) <= 0.0f)
		{
			break;
		}

		// only forward side is emitable
		//if(hitSurfaceBehavior.getEmitter() && V.dot(hitDetail.getShadingNormal()) > 0.0_r)
		if(hitSurfaceBehavior.getEmitter())
		{
			SpectralStrength radianceLi;
			hitSurfaceBehavior.getEmitter()->evalEmittedRadiance(surfaceHit, &radianceLi);

			// avoid excessive, negative weight and possible NaNs
			//accuLiWeight.clampLocal(0.0f, 1000.0f);

			// avoid excessive, negative weight and possible NaNs
			accuLiWeight.clampLocal(0.0_r, 1000000000.0_r);
			if(accuLiWeight.isZero())
			{
				break;
			}

			accuRadiance.addLocal(radianceLi.mul(accuLiWeight));
		}

		///////////////////////////////////////////////////////////////////////////////
		// sample BSDF

		BsdfSample bsdfSample;
		bsdfSample.inputs.set(surfaceHit, V);
		hitSurfaceBehavior.getOptics()->genBsdfSample(bsdfSample);

		const Vector3R& N = hitDetail.getShadingNormal();
		const Vector3R& L = bsdfSample.outputs.L;

		// blackness check & sidedness agreement between real geometry and shading (phong-interpolated) normal
		if(!bsdfSample.outputs.isGood() ||
		   hitDetail.getShadingNormal().dot(L) * hitDetail.getGeometryNormal().dot(L) <= 0.0_r)
		{
			break;
		}

		switch(bsdfSample.outputs.phenomenon)
		{
		case ESurfacePhenomenon::REFLECTION:
		case ESurfacePhenomenon::TRANSMISSION:
		{
			//rayOriginDelta.set(L).mulLocal(rayDeltaDist);

			SpectralStrength liWeight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));

			if(numBounces >= 3)
			{
				//const real rrSurviveRate = liWeight.clamp(0.0f, 1.0f).max();
				//const real rrSurviveRate = Math::clamp(liWeight.avg(), 0.1_r, 1.0_r);
				const real rrSurviveRate = Math::clamp(liWeight.calcLuminance(EQuantity::EMR), 0.1_r, 1.0_r);
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
			std::cerr << "warning: unknown surface phenomenon type in BVPTIntegrator detected" << std::endl;
			keepSampling = false;
			break;
		}// end switch surface sample type

		if(!keepSampling)
		{
			break;
		}

		// volume test
		{
			const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
			if(surfaceHit.hasInteriorOptics() && surfaceHit.getShadingNormal().dot(V) * surfaceHit.getShadingNormal().dot(L) < 0.0_r)
			{
				SurfaceHit Xe;
				Vector3R endV;
				SpectralStrength weight;
				SpectralStrength radiance;
				PtVolumetricEstimator::sample(scene, surfaceHit, L, &Xe, &endV, &weight, &radiance);

				accuLiWeight.mulLocal(weight);
				if(accuLiWeight.isZero())
				{
					break;
				}

				BsdfSample bsdfSample;
				bsdfSample.inputs.set(Xe, endV);
				metadata->getSurface().getOptics()->genBsdfSample(bsdfSample);
				if(!bsdfSample.outputs.isGood())
				{
					break;
				}

				accuLiWeight.mulLocal(bsdfSample.outputs.pdfAppliedBsdf);
				if(accuLiWeight.isZero())
				{
					break;
				}

				const Vector3R nextRayOrigin(Xe.getPosition());
				const Vector3R nextRayDirection(bsdfSample.outputs.L);
				tracingRay.setOrigin(nextRayOrigin);
				tracingRay.setDirection(nextRayDirection);
			}
			else
			{
				const Vector3R nextRayOrigin(hitDetail.getPosition());
				const Vector3R nextRayDirection(L);
				tracingRay.setOrigin(nextRayOrigin);
				tracingRay.setDirection(nextRayDirection);
			}
		}
		numBounces++;

		// prepare for next iteration
		//const Vector3R nextRayOrigin(hitDetail.getPosition().add(hitDetail.getGeometryNormal().mul(0.001_r)));
		/*const Vector3R nextRayOrigin(hitDetail.getPosition());
		const Vector3R nextRayDirection(L);
		tracingRay.setOrigin(nextRayOrigin);
		tracingRay.setDirection(nextRayDirection);
		numBounces++;*/
	}// end while

	out_senseEvents.push_back(SenseEvent(/*sample.m_cameraX, sample.m_cameraY, */accuRadiance));
}

// command interface

BVPTIntegrator::BVPTIntegrator(const InputPacket& packet) :
	Integrator(packet)
{}

SdlTypeInfo BVPTIntegrator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "bvpt");
}

void BVPTIntegrator::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<BVPTIntegrator>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<BVPTIntegrator> BVPTIntegrator::ciLoad(const InputPacket& packet)
{
	return std::make_unique<BVPTIntegrator>(packet);
}

}// end namespace ph