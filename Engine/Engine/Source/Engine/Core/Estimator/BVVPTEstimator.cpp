#include "Engine/Core/Estimator/BVVPTEstimator.h"
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
#include "Engine/Core/LTA/DirectLightEstimator.h"
#include "Engine/Core/LTA/RussianRoulette.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/Estimator/Integrand.h"
#include "Engine/Core/VolumeBehavior/VolumeOptics.h"
#include "Engine/Core/VolumeBehavior/MediumDistanceSampleQuery.h"
#include "Engine/Core/LTA/VolumeTracker.h"

#include <optional>

namespace ph
{

void BVVPTEstimator::update(const Integrand& integrand)
{}

std::string BVVPTEstimator::toString() const
{
	return "BVVPT (Backward Vanilla Volumetric Path Tracing Estimator)";
}

std::unique_ptr<TIRayEstimator<math::Spectrum>> BVVPTEstimator::makeCopy() const
{
	return std::make_unique<BVVPTEstimator>(*this);
}

void BVVPTEstimator::estimate(
	const Ray&        ray,
	const Integrand&  integrand,
	SampleFlow&       sampleFlow,
	EnergyEstimation& out_estimation)
{
	constexpr auto sidednessPolicy = lta::ESidednessPolicy::Strict;

	// Transport tools
	const lta::SidednessAgreement sidedness{sidednessPolicy};
	const lta::RussianRoulette rr{};
	const lta::SurfaceTracer surfaceTracer{&(integrand.getScene())};
	const lta::DirectLightEstimator directLight{&integrand.getScene(), sidedness};

	// Common variables
	uint32 pathLength = 0;
	math::Spectrum pathEnergy(0);
	math::Spectrum pathThroughput(1);
	real rrScale = 1.0_r;

	// Backward tracing to light
	Ray tracingRay = Ray(ray).reverse();
	tracingRay.setRange(0, std::numeric_limits<real>::max());

	BsdfQueryContext bsdfContext{sidednessPolicy};
	bsdfContext.key = BsdfKey::makeRandom();
	lta::VolumeTracker volumeTracker{};

	SurfaceHit X;
	SurfaceHit nextX;
	while(true)
	{
		if(pathLength == 0)
		{
			Ray boundedRay;
			const bool foundGeometry = surfaceTracer.traceNextSurface(
				tracingRay, volumeTracker, &X, &boundedRay);
			if(foundGeometry)
			{
				sidedness.adjustForSidednessAgreement(X);
			}

			pathEnergy += directLight.accumulateSurfaceEmission<EEmitterFeatureSet::ZeroBounceSample>(
				boundedRay,
				nullptr,
				sampleFlow,
				[](const SurfaceHit& Xe)
				{
					math::Spectrum radianceLe;
					Xe.getSurfaceEmitter().evalEmittedEnergy(Xe, &radianceLe);
					return radianceLe;
				});
			if(!foundGeometry || !sidedness.isSidednessAgreed(X, boundedRay.getDir()))
			{
				break;
			}
		}

		const auto emitterFeature = pathLength == 0
			? EEmitterFeatureSet::ZeroBounceSample
			: EEmitterFeatureSet::BsdfSample;

		// FIXME: also update in volume rendering part
		++pathLength;

		const PrimitiveMetadata& metadata = X.getMetadata();
		const SurfaceBehavior& hitSurfaceBehavior = metadata.getSurface();

		if(hitSurfaceBehavior.isEmissive() &&
		   X.getSurfaceEmitter().getFeatureSet().has(emitterFeature))
		{
			math::Spectrum radianceLe;
			hitSurfaceBehavior.getEmitter().evalEmittedEnergy(X, &radianceLe);
			pathEnergy += radianceLe * pathThroughput;
		}

		// The next segment would exceed the requested path length
		if(pathLength > getPTParams().maxPathLength)
		{
			break;
		}

		const math::Vector3R V = tracingRay.getDir().mul(-1);
		const math::Vector3R N = X.getShadingNormal();

		BsdfSampleQuery bsdfSample{bsdfContext, X, V};
		Ray nextRay;
		if(!surfaceTracer.doBsdfSample(bsdfSample, sampleFlow, &nextRay))
		{
			break;
		}
		const math::Vector3R L = nextRay.getDir();

		pathThroughput *= bsdfSample.outputs.getPdfAppliedBsdfCos();
		if(pathThroughput.isZero())
		{
			break;
		}

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

		if(sidedness.isOppositeHemisphere(X, V, L))
		{
			if(N.dot(V) > 0)
			{
				volumeTracker.enterSurface(X);
			}
			else
			{
				volumeTracker.exitSurface(X);
			}
		}

		const VolumeOptics* volumeOptics = volumeTracker.getCurrentVolumeOptics();

		Ray boundedRay;
		const bool foundGeometry = surfaceTracer.traceNextSurfaceFrom(
			X,
			nextRay,
			volumeTracker,
			&nextX,
			&boundedRay);
		if(foundGeometry)
		{
			sidedness.adjustForSidednessAgreement(nextX);
		}

		// Volumetric transport is evaluated to each emitting surface's distance
		pathEnergy += directLight.accumulateSurfaceEmission<EEmitterFeatureSet::BsdfSample>(
			boundedRay,
			nullptr,
			sampleFlow,
			[&X, &nextRay, &N, &L, &sampleFlow, &pathThroughput, volumeOptics](const SurfaceHit& Xe)
			{
				math::Spectrum radianceLe;
				Xe.getSurfaceEmitter().evalEmittedEnergy(Xe, &radianceLe);
				if(radianceLe.isZero())
				{
					return math::Spectrum(0);
				}

				math::Spectrum weight = pathThroughput;
				if(volumeOptics)
				{
					// Volume sampling starts at the departing surface
					const bool isFrontHemisphere = N.dot(L) > 0;
					const VolumeHit volumeHit(X, nextRay, !isFrontHemisphere);

					MediumDistanceSampleQuery distanceSample;
					distanceSample.inputs.set(volumeHit, L, (Xe.getPos() - X.getPos()).length());
					volumeOptics->genDistanceSample(distanceSample, sampleFlow);
					if(!distanceSample.outputs)
					{
						return math::Spectrum(0);
					}

					weight *= distanceSample.outputs.getPdfAppliedWeight();
				}
				
				weight.safeClampLocal(0.0_r, 1e9_r);
				return radianceLe * weight;
			});

		if(!foundGeometry || !sidedness.isSidednessAgreed(nextX, boundedRay.getDir()))
		{
			break;
		}

		// Volumetric transport
		if(volumeOptics)
		{
			const auto penetrationDepth = (nextX.getPos() - X.getPos()).length();
			const bool isFrontHemisphere = N.dot(L) > 0;
			const VolumeHit volumeHit(X, nextRay, !isFrontHemisphere);

			MediumDistanceSampleQuery distanceSample;
			distanceSample.inputs.set(
				volumeHit,
				L,
				penetrationDepth);
			volumeOptics->genDistanceSample(distanceSample, sampleFlow);
			if(!distanceSample.outputs)
			{
				break;
			}

			/*if(pathLength > 20)
			{
				PH_DEFAULT_DEBUG_LOG_ONCE("ttt");
			}*/

			pathThroughput *= distanceSample.outputs.getPdfAppliedWeight();
		}

		// Avoid excessive, negative weight and possible NaNs
		pathThroughput.safeClampLocal(0.0_r, 1e9_r);

		if(pathThroughput.isZero())
		{
			break;
		}

		// Will extend the path, update states for next bounce
		X = nextX;
		tracingRay = nextRay;
		bsdfContext.key = bsdfContext.key.makeRandom();
	}// end while

	out_estimation[getPathEnergyIndex()] = pathEnergy;
}

}// end namespace ph
