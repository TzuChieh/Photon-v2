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
	// Transport tools
	const lta::RussianRoulette rr{};
	const lta::SurfaceTracer surfaceTracer{&(integrand.getScene())};

	// Common variables
	uint32 numBounces = 0;
	math::Spectrum pathEnergy(0);
	math::Spectrum pathThroughput(1);
	real rrScale = 1.0_r;

	// Backward tracing to light
	Ray tracingRay = Ray(ray).reverse();
	tracingRay.setRange(0, std::numeric_limits<real>::max());

	SurfaceHit surfaceHit;
	while(numBounces <= MAX_RAY_BOUNCES)
	{
		if(numBounces == 0)
		{
			if(!surfaceTracer.traceNextSurface(tracingRay, BsdfQueryContext{}.sidedness, &surfaceHit))
			{
				break;
			}
		}
		else
		{
			if(!surfaceTracer.traceNextSurfaceFrom(
				surfaceHit, tracingRay, BsdfQueryContext{}.sidedness, &surfaceHit))
			{
				break;
			}
		}

		const auto* const      metadata           = surfaceHit.getDetail().getPrimitive()->getMetadata();
		const SurfaceBehavior& hitSurfaceBehavior = metadata->getSurface();

		if(hitSurfaceBehavior.getEmitter())
		{
			math::Spectrum radianceLe;
			hitSurfaceBehavior.getEmitter()->evalEmittedEnergy(surfaceHit, &radianceLe);

			// Avoid excessive, negative weight and possible NaNs
			pathThroughput.safeClampLocal(0.0_r, 1e9_r);

			pathEnergy += radianceLe * pathThroughput;
		}

		const math::Vector3R V = tracingRay.getDir().mul(-1);
		const math::Vector3R N = surfaceHit.getShadingNormal();

		BsdfSampleQuery bsdfSample;
		bsdfSample.inputs.set(surfaceHit, V);
		Ray nextRay;
		if(!surfaceTracer.doBsdfSample(bsdfSample, sampleFlow, &nextRay))
		{
			break;
		}

		pathThroughput *= bsdfSample.outputs.getPdfAppliedBsdfCos();

		// Prevent premature termination of the path due to solid angle compression/expansion
		rrScale /= bsdfSample.outputs.getRelativeIor2();

		if(numBounces >= 3)
		{
			real rrSurvivalProb;
			if(rr.surviveOnLuminance(pathThroughput * rrScale, sampleFlow, &rrSurvivalProb))
			{
				pathThroughput *= 1.0_r / rrSurvivalProb;
			}
			else
			{
				break;
			}
		}

		if(pathThroughput.isZero())
		{
			break;
		}

		// volume test
		{
			const math::Vector3R L = bsdfSample.outputs.getL();

			const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
			if(surfaceHit.hasInteriorOptics() && surfaceHit.getShadingNormal().dot(V) * surfaceHit.getShadingNormal().dot(L) < 0.0_r)
			{
				SurfaceHit Xe;
				math::Vector3R endV;
				math::Spectrum weight;
				math::Spectrum radiance;
				lta::PtVolumetricEstimator::sample(integrand.getScene(), surfaceHit, L, &Xe, &endV, &weight, &radiance);

				pathThroughput.mulLocal(weight);
				if(pathThroughput.isZero())
				{
					break;
				}

				BsdfSampleQuery bsdfSample;
				bsdfSample.inputs.set(Xe, endV);
				metadata->getSurface().getOptics()->genBsdfSample(bsdfSample, sampleFlow);
				if(!bsdfSample.outputs.isMeasurable())
				{
					break;
				}

				// XXX: cosine term?
				pathThroughput.mulLocal(bsdfSample.outputs.getPdfAppliedBsdf());
				if(pathThroughput.isZero())
				{
					break;
				}

				const math::Vector3R nextRayOrigin(Xe.getPos());
				const math::Vector3R nextRayDir(bsdfSample.outputs.getL());
				tracingRay.setOrigin(nextRayOrigin);
				tracingRay.setDir(nextRayDir);
			}
			else
			{
				tracingRay = nextRay;
			}
		}
		numBounces++;
	}// end while

	out_estimation[m_estimationIndex] = pathEnergy;
}

}// end namespace ph
