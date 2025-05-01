#include "Engine/Core/Estimator/BVPTEstimator.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/Emitter/Emitter.h"
#include "Engine/Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/LTA/SurfaceTracer.h"
#include "Engine/Core/LTA/RussianRoulette.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/Estimator/Integrand.h"

#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 1

namespace ph
{

void BVPTEstimator::update(const Integrand& integrand)
{}

std::string BVPTEstimator::toString() const
{
	return "Backward Vanilla Path Tracing Estimator";
}

std::unique_ptr<TIRayEstimator<math::Spectrum>> BVPTEstimator::makeCopy() const
{
	return std::make_unique<BVPTEstimator>(*this);
}

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
		//{
		//	const math::Vector3R L = bsdfSample.outputs.getL();

		//	const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
		//	if(surfaceHit.hasInteriorOptics() && surfaceHit.getShadingNormal().dot(V) * surfaceHit.getShadingNormal().dot(L) < 0.0_r)
		//	{
		//		SurfaceHit Xe;
		//		math::Vector3R endV;
		//		math::Spectrum weight;
		//		math::Spectrum radiance;
		//		lta::PtVolumetricEstimator::sample(integrand.getScene(), surfaceHit, L, &Xe, &endV, &weight, &radiance);

		//		pathThroughput.mulLocal(weight);
		//		if(pathThroughput.isZero())
		//		{
		//			break;
		//		}

		//		BsdfSampleQuery bsdfSample;
		//		bsdfSample.inputs.set(Xe, endV);
		//		metadata->getSurface().getOptics()->genBsdfSample(bsdfSample, sampleFlow);
		//		if(!bsdfSample.outputs.isMeasurable())
		//		{
		//			break;
		//		}

		//		// XXX: cosine term?
		//		pathThroughput.mulLocal(bsdfSample.outputs.getPdfAppliedBsdf());
		//		if(pathThroughput.isZero())
		//		{
		//			break;
		//		}

		//		const math::Vector3R nextRayOrigin(Xe.getPos());
		//		const math::Vector3R nextRayDir(bsdfSample.outputs.getL());
		//		tracingRay.setOrigin(nextRayOrigin);
		//		tracingRay.setDir(nextRayDir);
		//	}
		//	else
		//	{
		//		tracingRay = nextRay;
		//	}
		//}
		numBounces++;
	}// end while

	out_estimation[m_estimationIdx] = pathEnergy;
}

}// end namespace ph
