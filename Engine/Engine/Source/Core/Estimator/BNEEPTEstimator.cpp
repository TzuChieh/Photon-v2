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
#include "Core/LTA/TMIS.h"
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
	const BsdfQueryContext bsdfContext{sidednessPolicy};

	// Transport tools
	const lta::TDirectLightEstimator<sidednessPolicy> directLight{&integrand.getScene()};
	const lta::TMIS<lta::EMISStyle::Power> mis{};
	const lta::RussianRoulette rr{};
	const lta::SurfaceTracer surfaceTracer{&integrand.getScene()};
	const lta::SidednessAgreement sidedness{sidednessPolicy};

	// Common variables
	math::Spectrum pathEnergy(0);
	math::Spectrum pathThroughput(1);
	SurfaceHit X;
	real rrScale = 1.0_r;

	// Reversing the ray for backward tracing
	Ray tracingRay = Ray(ray).reverse();
	tracingRay.setRange(0, std::numeric_limits<real>::max());

	if(!surfaceTracer.traceNextSurface(tracingRay, sidedness, &X))
	{
		out_estimation[m_estimationIndex] = pathEnergy;
		return;
	}

	// 0-bounce direct lighting
	{
		PH_SCOPED_TIMER(ZeroBounceDirect);
		
		math::Spectrum radianceLe;
		if(surfaceTracer.sampleZeroBounceEmission(X, sidedness, &radianceLe))
		{
			pathEnergy.addLocal(radianceLe);
		}
	}

	// Ray bouncing around the scene (1 ~ N bounces)
	for(uint32 numBounces = 0; numBounces < MAX_RAY_BOUNCES; numBounces++)
	{
		// Optics must present
		const SurfaceOptics* surfaceOptics = X.getSurfaceOptics();
		if(!surfaceOptics)
		{
			break;
		}

		const math::Vector3R V = tracingRay.getDir().mul(-1.0_r);
		PH_ASSERT_MSG(V.isFinite(), V.toString());

		const bool useNeeLightSampling = directLight.isNeeSamplable(X);
		const bool useBsdfLightSampling = true;

		// Must use at least one of the techniques to avoid bias
		PH_ASSERT(useNeeLightSampling || useBsdfLightSampling);

		// Sample light with NEE
		if(useNeeLightSampling)
		{
			PH_SCOPED_TIMER(DirectLightSampling);

			DirectEnergySampleQuery directSample;
			directSample.inputs.set(X);
			SurfaceHit Xe;
			if(directLight.neeSampleEmission(directSample, sampleFlow, &Xe) &&
			   directSample.outputs)
			{
				const auto L = directSample.getTargetToEmit().normalize();
				const Emitter* directEmitter = Xe.getSurfaceEmitter();

				BsdfEvalQuery bsdfEval(bsdfContext);
				bsdfEval.inputs.set(X, L, V);
				surfaceOptics->calcBsdf(bsdfEval);
				if(bsdfEval.outputs.isMeasurable())
				{
					// MIS: NEE + BSDF sample

					real bsdfSamplePdfW = 0.0_r;
					if(useBsdfLightSampling &&
					   directEmitter &&
					   directEmitter->getFeatureSet().has(EEmitterFeatureSet::BsdfSample))
					{
						BsdfPdfQuery bsdfPdfQuery(bsdfContext);
						bsdfPdfQuery.inputs.set(bsdfEval.inputs);
						surfaceOptics->calcBsdfPdf(bsdfPdfQuery);

						bsdfSamplePdfW = bsdfPdfQuery.outputs
							? bsdfPdfQuery.outputs.getSampleDirPdfW() : 0.0_r;
					}

					const real misWeighting = mis.weight(directSample.outputs.getPdfW(), bsdfSamplePdfW);
					const math::Vector3R N = X.getShadingNormal();

					math::Spectrum weight = bsdfEval.outputs.getBsdf().mul(N.absDot(L));
					weight *= pathThroughput;
					weight *= misWeighting / directSample.outputs.getPdfW();

					// Avoid excessive, negative weight and possible NaNs
					rationalClamp(weight);

					pathEnergy += directSample.outputs.getEmittedEnergy() * weight;
				}
			}
		}// end direct light sample

		// Extend the path with BSDF sampling + sample light simultaneously
		{
			PH_SCOPED_TIMER(BSDFAndIndirectLightSampling);

			BsdfSampleQuery bsdfSample(bsdfContext);
			bsdfSample.inputs.set(X, V);
			surfaceOptics->genBsdfSample(bsdfSample, sampleFlow);
			if(!bsdfSample.outputs.isMeasurable())
			{
				break;
			}

			const math::Vector3R N = X.getShadingNormal();
			const math::Vector3R L = bsdfSample.outputs.getL();

			PH_ASSERT_MSG(L.isFinite(),
				"L = " + L.toString() + ", from " + surfaceOptics->toString());

			// Trace a ray using BSDF's suggestion
			tracingRay.setOrigin(X.getPos());
			tracingRay.setDir(L);
			SurfaceHit nextX;
			if(!surfaceTracer.traceNextSurfaceFrom(X, tracingRay, sidedness, &nextX))
			{
				break;
			}

			const Emitter* nextEmitter = nextX.getSurfaceEmitter();
			if(useBsdfLightSampling &&
			   nextEmitter &&
			   nextEmitter->getFeatureSet().has(EEmitterFeatureSet::BsdfSample))
			{
				math::Spectrum radianceLe;
				nextEmitter->evalEmittedEnergy(nextX, &radianceLe);

				// TODO: not doing MIS if delta elemental exists is too harsh--we can do regular sample for
				// deltas and MIS for non-deltas

				// MIS: BSDF sample + NEE
				if(useNeeLightSampling &&
				   !radianceLe.isZero())
				{
					// `directLightPdfW` can be 0 (e.g., delta BSDF) and this is fine--MIS weighting
					// still works. No need to test occlusion again as we already done that.
					const real directLightPdfW = directLight.neeSamplePdfWUnoccluded(X, nextX);

					BsdfPdfQuery bsdfPdfQuery;
					bsdfPdfQuery.inputs.set(bsdfSample);
					surfaceOptics->calcBsdfPdf(bsdfPdfQuery);

					// `isNeeSamplable()` is already `true`, but BSDF PDF can still be empty or 0
					// (e.g., sidedness policy or by the distribution itself)
					if(bsdfPdfQuery.outputs)
					{
						const real bsdfSamplePdfW = bsdfPdfQuery.outputs.getSampleDirPdfW();
						const real misWeighting = mis.weight(bsdfSamplePdfW, directLightPdfW);

						math::Spectrum weight = bsdfSample.outputs.getPdfAppliedBsdfCos();
						weight *= pathThroughput;
						weight *= misWeighting;

						// Avoid excessive, negative weight and possible NaNs
						rationalClamp(weight);

						pathEnergy += radianceLe * weight;
					}
				}
				// No MIS: BSDF sample only
				else
				{
					math::Spectrum weight = bsdfSample.outputs.getPdfAppliedBsdfCos();
					weight *= pathThroughput;

					// Avoid excessive, negative weight and possible NaNs
					rationalClamp(weight);

					pathEnergy += radianceLe * weight;
				}
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

			X = nextX;
		}
	}// end for each bounces

	PH_ASSERT_MSG(pathThroughput.isFinite() && pathEnergy.isFinite(),
		"pathThroughput = " + pathThroughput.toString() + ", pathEnergy = " + pathEnergy.toString());

	out_estimation[m_estimationIndex] = pathEnergy;
}

void BNEEPTEstimator::rationalClamp(math::Spectrum& value)
{
	// TODO: should negative value be allowed?
	value.safeClampLocal(0.0_r, 1e9_r);
}

}// end namespace ph
