#include "Core/Estimator/BNEEPTEstimator.h"
#include "Core/Estimator/Integrand.h"
#include "Core/Ray.h"
#include "World/Scene.h"
#include "Math/TVector3.h"
#include "Core/HitDetail.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Math/math.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Math/Color/Spectrum.h"
#include "Core/LTA/TMis.h"
#include "Core/LTA/TDirectLightEstimator.h"
#include "Core/LTA/RussianRoulette.h"
#include "Core/LTA/SurfaceTracer.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/stats.h>

#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 1

namespace ph
{

PH_DEFINE_INTERNAL_TIMER_STAT(FullEstimation, Render_BNEEPTEstimator);
PH_DEFINE_INTERNAL_TIMER_STAT(ZeroBounceDirect, Render_BNEEPTEstimator);
PH_DEFINE_INTERNAL_TIMER_STAT(DirectLightSampling, Render_BNEEPTEstimator);
PH_DEFINE_INTERNAL_TIMER_STAT(BSDFAndIndirectLightSampling, Render_BNEEPTEstimator);

void BNEEPTEstimator::estimate(
	const Ray&        ray,
	const Integrand&  integrand,
	SampleFlow&       sampleFlow,
	EnergyEstimation& out_estimation) const
{
	PH_SCOPED_TIMER(FullEstimation);

	constexpr auto sidednessPolicy = lta::ESidednessPolicy::Strict;

	const lta::TDirectLightEstimator<sidednessPolicy> directLight{&integrand.getScene()};
	const lta::TMis<lta::EMisStyle::Power> mis{};
	const lta::RussianRoulette rr{};
	const lta::SurfaceTracer surfaceTracer{&integrand.getScene()};
	const lta::SidednessAgreement sidedness{sidednessPolicy};
	const BsdfQueryContext bsdfContext{sidednessPolicy};

	// Common variables
	math::Spectrum accuRadiance(0);
	math::Spectrum accuLiWeight(1);
	SurfaceHit     surfaceHit;

	// Reversing the ray for backward tracing
	Ray tracingRay = Ray(ray).reverse();
	tracingRay.setRange(0, std::numeric_limits<real>::max());

	if(!surfaceTracer.traceNextSurface(tracingRay, sidedness, &surfaceHit))
	{
		out_estimation[m_estimationIndex] = accuRadiance;
		return;
	}

	// 0-bounce direct lighting
	{
		PH_SCOPED_TIMER(ZeroBounceDirect);
		
		math::Spectrum radianceLi;
		if(surfaceTracer.sampleZeroBounceEmission(surfaceHit, sidedness, &radianceLi))
		{
			accuRadiance.addLocal(radianceLi);
		}
	}

	// Ray bouncing around the scene (1 ~ N bounces)
	for(uint32 numBounces = 0; numBounces < MAX_RAY_BOUNCES; numBounces++)
	{
		const math::Vector3R V = tracingRay.getDirection().mul(-1.0_r);
		PH_ASSERT_MSG(V.isFinite(), V.toString());

		const bool canDoNEE = directLight.isNeeSamplable(surfaceHit);

		// Direct light sample
		{
			PH_SCOPED_TIMER(DirectLightSampling);

			DirectEnergySampleQuery directSample;
			directSample.inputs.set(surfaceHit);
			if(canDoNEE && 
			   directLight.neeSampleEmission(directSample, sampleFlow) &&
			   directSample.outputs)
			{
				// With a contributing NEE sample, optics must present
				const SurfaceOptics* surfaceOptics = surfaceHit.getSurfaceOptics();
				PH_ASSERT(surfaceOptics);

				const auto L = directSample.getTargetToEmit().normalize();

				BsdfEvalQuery bsdfEval(bsdfContext);
				bsdfEval.inputs.set(surfaceHit, L, V);
				surfaceOptics->calcBsdf(bsdfEval);
				if(bsdfEval.outputs.isMeasurable())
				{
					BsdfPdfQuery bsdfPdfQuery(bsdfContext);
					bsdfPdfQuery.inputs.set(bsdfEval.inputs);
					surfaceOptics->calcBsdfSamplePdfW(bsdfPdfQuery);

					const real bsdfSamplePdfW = bsdfPdfQuery.outputs.getSampleDirPdfW();
					const real misWeighting = mis.weight(directSample.outputs.getPdfW(), bsdfSamplePdfW);
					const math::Vector3R N = surfaceHit.getShadingNormal();

					math::Spectrum weight;
					weight = bsdfEval.outputs.getBsdf().mul(N.absDot(L));
					weight.mulLocal(accuLiWeight).mulLocal(misWeighting / directSample.outputs.getPdfW());

					// Avoid excessive, negative weight and possible NaNs
					rationalClamp(weight);

					accuRadiance.addLocal(directSample.outputs.getEmittedEnergy() * weight);
				}
			}
		}// end direct light sample

		// BSDF sample + indirect light sample
		{
			PH_SCOPED_TIMER(BSDFAndIndirectLightSampling);

			const SurfaceOptics* surfaceOptics = surfaceHit.getSurfaceOptics();
			PH_ASSERT(surfaceOptics);

			BsdfSampleQuery bsdfSample(bsdfContext);
			bsdfSample.inputs.set(surfaceHit, V);
			surfaceOptics->calcBsdfSample(bsdfSample, sampleFlow);
			if(!bsdfSample.outputs.isMeasurable())
			{
				break;
			}

			const math::Vector3R N = surfaceHit.getShadingNormal();
			const math::Vector3R L = bsdfSample.outputs.getL();

			PH_ASSERT_MSG(L.isFinite(),
				"L = " + L.toString() + ", from " + surfaceOptics->toString());

			//BsdfPdfQuery bsdfPdfQuery;
			//bsdfPdfQuery.inputs.set(bsdfSample);
			//surfaceBehavior->getOptics()->calcBsdfSamplePdfW(bsdfPdfQuery);

			//const real bsdfSamplePdfW = bsdfPdfQuery.outputs.sampleDirPdfW;

			//// TODO: We should break right after bsdf sampling if the sample 
			//// is invalid, but current implementation applied pdf on bsdf
			//// beforehand, which might hide cases where pdf = 0.
			//if(bsdfSamplePdfW == 0.0_r)
			//{
			//	break;
			//}

			// Trace a ray using BSDF's suggestion
			tracingRay.setOrigin(surfaceHit.getPosition());
			tracingRay.setDirection(L);
			SurfaceHit nextSurfaceHit;
			if(!surfaceTracer.traceNextSurfaceFrom(surfaceHit, tracingRay, sidedness, &nextSurfaceHit))
			{
				break;
			}

			const Emitter* nextEmitter = nextSurfaceHit.getSurfaceEmitter();
			if(nextEmitter)
			{
				math::Spectrum radianceLe;
				nextEmitter->evalEmittedRadiance(nextSurfaceHit, &radianceLe);

				// TODO: not doing MIS if delta elemental exists is too harsh--we can do regular sample for
				// deltas and MIS for non-deltas

				// Do MIS (BSDF sample + NEE)
				if(canDoNEE && !radianceLe.isZero())
				{
					// TODO: <directLightPdfW> might be 0, should we stop using MIS if one of two 
					// sampling techniques has failed?
					// <bsdfSamplePdfW> can also be 0 for delta distributions
					// `directLightPdfW` can be 0 (e.g., delta BSDF) and this is fine--MIS weighting
					// still works. No need to test occlusion again as we already done that.
					const real directLightPdfW = directLight.neeSamplePdfWUnoccluded(
						surfaceHit, nextSurfaceHit);

					BsdfPdfQuery bsdfPdfQuery;
					bsdfPdfQuery.inputs.set(bsdfSample);
					surfaceOptics->calcBsdfSamplePdfW(bsdfPdfQuery);

					// `canDoNEE` is already checked, but `bsdfSamplePdfW` can still be 0 (e.g.,
					// sidedness policy or by the distribution itself).
					const real bsdfSamplePdfW = bsdfPdfQuery.outputs.getSampleDirPdfW();
					if(bsdfSamplePdfW > 0)
					{
						const real misWeighting = mis.weight(bsdfSamplePdfW, directLightPdfW);

						math::Spectrum weight = bsdfSample.outputs.getPdfAppliedBsdf().mul(N.absDot(L));
						weight.mulLocal(accuLiWeight).mulLocal(misWeighting);

						// Avoid excessive, negative weight and possible NaNs
						rationalClamp(weight);

						accuRadiance.addLocal(radianceLe.mulLocal(weight));
					}
				}
				// Not do MIS (BSDF sample only)
				else
				{
					math::Spectrum weight = bsdfSample.outputs.getPdfAppliedBsdf().mul(N.absDot(L));
					weight.mulLocal(accuLiWeight);

					accuRadiance.addLocal(radianceLe.mulLocal(weight));
				}
			}

			const math::Spectrum currentLiWeight = bsdfSample.outputs.getPdfAppliedBsdf().mul(N.absDot(L));
			accuLiWeight.mulLocal(currentLiWeight);

			if(numBounces >= 3)
			{
				math::Spectrum weightedAccuLiWeight;
				if(rr.surviveOnLuminance(
					accuLiWeight, sampleFlow, &weightedAccuLiWeight))
				{
					accuLiWeight = weightedAccuLiWeight;
				}
				else
				{
					break;
				}
			}

			// Avoid excessive, negative weight and possible NaNs
			rationalClamp(accuLiWeight);

			if(accuLiWeight.isZero())
			{
				break;
			}

			surfaceHit = nextSurfaceHit;
		}
	}// end for each bounces

	PH_ASSERT_MSG(accuLiWeight.isFinite() && accuRadiance.isFinite(),
		"accuLiWeight = " + accuLiWeight.toString() + ", accuRadiance = " + accuRadiance.toString());

	out_estimation[m_estimationIndex] = accuRadiance;
}

void BNEEPTEstimator::rationalClamp(math::Spectrum& value)
{
	// TODO: should negative value be allowed?
	value.clampLocal(0.0_r, 1e9_r);
}

}// end namespace ph
