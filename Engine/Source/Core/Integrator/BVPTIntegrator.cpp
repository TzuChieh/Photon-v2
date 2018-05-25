#include "Core/Integrator/BVPTIntegrator.h"
#include "Core/Ray.h"
#include "Core/HitDetail.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Emitter/Emitter.h"
#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Integrator/Utility/PtVolumetricEstimator.h"
#include "Core/Integrator/Utility/TSurfaceEventDispatcher.h"
#include "Core/Integrator/Utility/RussianRoulette.h"
#include "FileIO/SDL/InputPacket.h"
#include "Math/TVector3.h"

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
	const auto& surfaceEventDispatcher = TSurfaceEventDispatcher<ESidednessAgreement::STRICT>(data.scene);

	uint32 numBounces = 0;
	SpectralStrength accuRadiance(0);
	SpectralStrength accuLiWeight(1);

	// backward tracing to light
	Ray tracingRay = Ray(ray).reverse();
	tracingRay.setMinT(0.0001_r);// HACK: hard-coded number
	tracingRay.setMaxT(std::numeric_limits<real>::max());

	SurfaceHit surfaceHit;
	while(numBounces <= MAX_RAY_BOUNCES && 
	      surfaceEventDispatcher.traceNextSurface(tracingRay, &surfaceHit))
	{
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
		Ray nextRay;
		if(!surfaceEventDispatcher.doBsdfSample(surfaceHit, bsdfSample, &nextRay))
		{
			break;
		}

		const Vector3R& N = hitDetail.getShadingNormal();
		const Vector3R& L = bsdfSample.outputs.L;

		switch(bsdfSample.outputs.phenomenon)
		{
		case ESurfacePhenomenon::REFLECTION:
		case ESurfacePhenomenon::TRANSMISSION:
		{
			//rayOriginDelta.set(L).mulLocal(rayDeltaDist);

			SpectralStrength liWeight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));

			if(numBounces >= 3)
			{
				SpectralStrength weightedLiWeight;
				RussianRoulette::surviveOnLuminance(liWeight, &weightedLiWeight);

				liWeight = weightedLiWeight;
			}

			accuLiWeight.mulLocal(liWeight);
		}
		break;

		default:
			std::cerr << "warning: unknown surface phenomenon type in BVPTIntegrator detected" << std::endl;
			accuLiWeight.setValues(0.0_r);
			break;
		}// end switch surface sample type

		if(accuLiWeight.isZero())
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
				PtVolumetricEstimator::sample(*(data.scene), surfaceHit, L, &Xe, &endV, &weight, &radiance);

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
				tracingRay = nextRay;
			}
		}
		numBounces++;
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