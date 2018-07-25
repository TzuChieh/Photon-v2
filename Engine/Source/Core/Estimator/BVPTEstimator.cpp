#include "Core/Estimator/BVPTEstimator.h"
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
#include "Core/Estimator/Utility/PtVolumetricEstimator.h"
#include "Core/Estimator/Utility/TSurfaceEventDispatcher.h"
#include "Core/Estimator/Utility/RussianRoulette.h"
#include "FileIO/SDL/InputPacket.h"
#include "Math/TVector3.h"
#include "Core/Quantity/SpectralStrength.h"

#include <iostream>

#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 1

namespace ph
{

BVPTEstimator::BVPTEstimator() = default;

BVPTEstimator::~BVPTEstimator() = default;

void BVPTEstimator::radianceAlongRay(
	const Ray&        ray,
	const Integrand&  integrand,
	SpectralStrength& out_radiance,
	SurfaceHit&       out_firstHit) const
{
	const auto& surfaceEventDispatcher = TSurfaceEventDispatcher<ESidednessAgreement::STRICT>(&(integrand.getScene()));

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
		if(numBounces == 0)
		{
			out_firstHit = surfaceHit;
		}

		const auto* const     metadata            = surfaceHit.getDetail().getPrimitive()->getMetadata();
		const SurfaceBehavior& hitSurfaceBehavior = metadata->getSurface();

		if(hitSurfaceBehavior.getEmitter())
		{
			SpectralStrength radianceLi;
			hitSurfaceBehavior.getEmitter()->evalEmittedRadiance(surfaceHit, &radianceLi);

			// avoid excessive, negative weight and possible NaNs
			accuLiWeight.clampLocal(0.0_r, 1000000000.0_r);

			accuRadiance.addLocal(radianceLi.mul(accuLiWeight));
		}

		const Vector3R V = tracingRay.getDirection().mul(-1.0f);
		const Vector3R N = surfaceHit.getShadingNormal();

		BsdfSample bsdfSample;
		bsdfSample.inputs.set(surfaceHit, V);
		Ray nextRay;
		if(!surfaceEventDispatcher.doBsdfSample(surfaceHit, bsdfSample, &nextRay))
		{
			break;
		}

		const Vector3R L = bsdfSample.outputs.L;

		SpectralStrength liWeight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));
		if(numBounces >= 3)
		{
			SpectralStrength weightedLiWeight;
			RussianRoulette::surviveOnLuminance(liWeight, &weightedLiWeight);

			liWeight = weightedLiWeight;
		}
		accuLiWeight.mulLocal(liWeight);

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
				PtVolumetricEstimator::sample(integrand.getScene(), surfaceHit, L, &Xe, &endV, &weight, &radiance);

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

	/*if(!accuRadiance.isZero())
	{
		int a = 3;
	}*/

	out_radiance = accuRadiance;
}

// command interface

BVPTEstimator::BVPTEstimator(const InputPacket& packet) :
	PathEstimator(packet)
{}

SdlTypeInfo BVPTEstimator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ESTIMATOR, "bvpt");
}

void BVPTEstimator::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<BVPTEstimator>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<BVPTEstimator> BVPTEstimator::ciLoad(const InputPacket& packet)
{
	return std::make_unique<BVPTEstimator>(packet);
}

}// end namespace ph