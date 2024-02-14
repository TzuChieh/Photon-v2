#include "Core/Estimator/BVPTEstimator.h"
#include "Core/Ray.h"
#include "Core/HitDetail.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/Intersection/Primitive.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Emitter/Emitter.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Math/Color/Spectrum.h"
#include "Core/LTA/PtVolumetricEstimator.h"
#include "Core/LTA/SurfaceTracer.h"
#include "Core/LTA/RussianRoulette.h"
#include "Math/TVector3.h"
#include "Core/Estimator/Integrand.h"

#include <iostream>

#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 1

namespace ph
{

void BVPTEstimator::estimate(
	const Ray&        ray,
	const Integrand&  integrand,
	SampleFlow&       sampleFlow,
	EnergyEstimation& out_estimation) const
{
	const lta::SurfaceTracer surfaceTracer{&(integrand.getScene())};

	uint32 numBounces = 0;
	math::Spectrum accuRadiance(0);
	math::Spectrum accuLiWeight(1);

	// backward tracing to light
	Ray tracingRay = Ray(ray).reverse();
	tracingRay.setMinT(0.0001_r);// HACK: hard-coded number
	tracingRay.setMaxT(std::numeric_limits<real>::max());

	SurfaceHit surfaceHit;
	while(numBounces <= MAX_RAY_BOUNCES && 
	      surfaceTracer.traceNextSurface(tracingRay, BsdfQueryContext().sidedness, &surfaceHit))
	{
		const auto* const      metadata           = surfaceHit.getDetail().getPrimitive()->getMetadata();
		const SurfaceBehavior& hitSurfaceBehavior = metadata->getSurface();

		if(hitSurfaceBehavior.getEmitter())
		{
			math::Spectrum radianceLi;
			hitSurfaceBehavior.getEmitter()->evalEmittedRadiance(surfaceHit, &radianceLi);

			// avoid excessive, negative weight and possible NaNs
			accuLiWeight.clampLocal(0.0_r, 1000000000.0_r);

			accuRadiance.addLocal(radianceLi.mul(accuLiWeight));
		}

		const math::Vector3R V = tracingRay.getDirection().mul(-1.0f);
		const math::Vector3R N = surfaceHit.getShadingNormal();

		BsdfSampleQuery bsdfSample;
		bsdfSample.inputs.set(surfaceHit, V);
		Ray nextRay;
		if(!surfaceTracer.doBsdfSample(bsdfSample, sampleFlow, &nextRay))
		{
			break;
		}

		const math::Vector3R L = bsdfSample.outputs.getL();
		const math::Spectrum liWeight = bsdfSample.outputs.getPdfAppliedBsdf().mul(N.absDot(L));
		accuLiWeight.mulLocal(liWeight);

		if(numBounces >= 3)
		{
			math::Spectrum weightedAccuLiWeight;
			if(lta::RussianRoulette{}.surviveOnLuminance(
				accuLiWeight, sampleFlow, &weightedAccuLiWeight))
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
				math::Spectrum weight;
				math::Spectrum radiance;
				lta::PtVolumetricEstimator::sample(integrand.getScene(), surfaceHit, L, &Xe, &endV, &weight, &radiance);

				accuLiWeight.mulLocal(weight);
				if(accuLiWeight.isZero())
				{
					break;
				}

				BsdfSampleQuery bsdfSample;
				bsdfSample.inputs.set(Xe, endV);
				metadata->getSurface().getOptics()->calcBsdfSample(bsdfSample, sampleFlow);
				if(!bsdfSample.outputs.isMeasurable())
				{
					break;
				}

				accuLiWeight.mulLocal(bsdfSample.outputs.getPdfAppliedBsdf());
				if(accuLiWeight.isZero())
				{
					break;
				}

				const math::Vector3R nextRayOrigin(Xe.getPosition());
				const math::Vector3R nextRayDirection(bsdfSample.outputs.getL());
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
