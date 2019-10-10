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
#include "Core/LTABuildingBlock/PtVolumetricEstimator.h"
#include "Core/LTABuildingBlock/TSurfaceEventDispatcher.h"
#include "Core/LTABuildingBlock/RussianRoulette.h"
#include "FileIO/SDL/InputPacket.h"
#include "Math/TVector3.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Estimator/Integrand.h"

#include <iostream>

#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 1

namespace ph
{

void BVPTEstimator::estimate(
	const Ray&        ray,
	const Integrand&  integrand,
	EnergyEstimation& out_estimation) const
{
	const auto& surfaceEventDispatcher = TSurfaceEventDispatcher<ESaPolicy::DO_NOT_CARE>(&(integrand.getScene()));

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

		const math::Vector3R V = tracingRay.getDirection().mul(-1.0f);
		const math::Vector3R N = surfaceHit.getShadingNormal();

		BsdfSample bsdfSample;
		bsdfSample.inputs.set(surfaceHit, V);
		Ray nextRay;
		if(!surfaceEventDispatcher.doBsdfSample(surfaceHit, bsdfSample, &nextRay))
		{
			break;
		}

		const math::Vector3R L = bsdfSample.outputs.L;

		const SpectralStrength liWeight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));
		accuLiWeight.mulLocal(liWeight);

		if(numBounces >= 3)
		{
			SpectralStrength weightedAccuLiWeight;
			if(RussianRoulette::surviveOnLuminance(
				accuLiWeight, &weightedAccuLiWeight))
			{
				accuLiWeight = weightedAccuLiWeight;
			}
			else
			{
				break;
			}
		}

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
				math::Vector3R endV;
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
				metadata->getSurface().getOptics()->calcBsdfSample(bsdfSample);
				if(!bsdfSample.outputs.isMeasurable())
				{
					break;
				}

				accuLiWeight.mulLocal(bsdfSample.outputs.pdfAppliedBsdf);
				if(accuLiWeight.isZero())
				{
					break;
				}

				const math::Vector3R nextRayOrigin(Xe.getPosition());
				const math::Vector3R nextRayDirection(bsdfSample.outputs.L);
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

	out_estimation[m_estimationIndex] = accuRadiance;
}

}// end namespace ph
