#include "Engine/Core/Estimator/BVPTEstimator.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/Emitter/SurfaceEmitter.h"
#include "Engine/Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/LTA/SurfaceTracer.h"
#include "Engine/Core/LTA/RussianRoulette.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/Estimator/Integrand.h"

#include <Common/logging.h>

namespace ph
{

void BVPTEstimator::setPTParams(PTEstimatorParams params)
{
	if(params.includeVolumetricEffects)
	{
		PH_DEFAULT_LOG(Warning,
			"{} cannot handle volumetric effects, will set "
			"`PTEstimatorParams::includeVolumetricEffects` to `false`", toString());

		params.includeVolumetricEffects = false;
	}

	PathEnergyEstimator::setPTParams(params);
}

void BVPTEstimator::update(const Integrand& integrand)
{}

std::string BVPTEstimator::toString() const
{
	return "BVPT (Backward Vanilla Path Tracing Estimator)";
}

std::unique_ptr<TIRayEstimator<math::Spectrum>> BVPTEstimator::makeCopy() const
{
	return std::make_unique<BVPTEstimator>(*this);
}

void BVPTEstimator::estimate(
	const Ray&        ray,
	const Integrand&  integrand,
	SampleFlow&       sampleFlow,
	EnergyEstimation& out_estimation)
{
	// Transport tools
	const lta::RussianRoulette rr{};
	const lta::SurfaceTracer surfaceTracer{&(integrand.getScene())};

	// Common variables
	uint32 pathLength = 0;
	math::Spectrum pathEnergy(0);
	math::Spectrum pathThroughput(1);
	real rrScale = 1.0_r;

	// Backward tracing to light
	Ray tracingRay = Ray(ray).reverse();
	tracingRay.setRange(0, std::numeric_limits<real>::max());

	SurfaceHit surfaceHit;
	while(pathLength <= getPTParams().maxPathLength)
	{
		if(pathLength == 0)
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

		++pathLength;

		const auto* const metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
		const SurfaceBehavior& hitSurfaceBehavior = metadata->getSurface();

		if(hitSurfaceBehavior.isEmissive())
		{
			math::Spectrum radianceLe;
			hitSurfaceBehavior.getEmitter().evalEmittedEnergy(surfaceHit, &radianceLe);

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

		if(pathLength >= getPTParams().rrBeginLengthHint)
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

		tracingRay = nextRay;
	}// end while

	out_estimation[getPathEnergyIndex()] = pathEnergy;
}

}// end namespace ph
