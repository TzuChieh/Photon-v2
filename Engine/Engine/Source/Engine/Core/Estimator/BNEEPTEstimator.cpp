#include "Engine/Core/Estimator/BNEEPTEstimator.h"
#include "Engine/Core/Estimator/Integrand.h"
#include "Engine/Core/Ray.h"
#include "Engine/World/Scene.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Math/math.h"
#include "Engine/Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/LTA/TMIS.h"
#include "Engine/Core/LTA/DirectLightEstimator.h"
#include "Engine/Core/LTA/RussianRoulette.h"
#include "Engine/Core/LTA/SurfaceTracer.h"
#include "Engine/Core/Emitter/Query/DirectEnergySampleQuery.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/stats.h>

#include <optional>

#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 1

namespace ph
{

PH_DEFINE_INTERNAL_TIMER_STAT(FullEstimation, Engine.Render.BNEEPTEstimator);
PH_DEFINE_INTERNAL_TIMER_STAT(ZeroBounceDirect, Engine.Render.BNEEPTEstimator.FullEstimation);
PH_DEFINE_INTERNAL_TIMER_STAT(DirectLightSampling, Engine.Render.BNEEPTEstimator.FullEstimation);
PH_DEFINE_INTERNAL_TIMER_STAT(BSDFAndIndirectLightSampling, Engine.Render.BNEEPTEstimator.FullEstimation);

void BNEEPTEstimator::update(const Integrand& integrand)
{}

std::string BNEEPTEstimator::toString() const
{
	return "BNEEPT (Backward NEE Path Tracing Estimator)";
}

std::unique_ptr<TIRayEstimator<math::Spectrum>> BNEEPTEstimator::makeCopy() const
{
	return std::make_unique<BNEEPTEstimator>(*this);
}

void BNEEPTEstimator::estimate(
	const Ray&        ray,
	const Integrand&  integrand,
	SampleFlow&       sampleFlow,
	EnergyEstimation& out_estimation)
{
	PH_SCOPED_TIMER(FullEstimation);

	constexpr auto sidednessPolicy = lta::ESidednessPolicy::Strict;

	// Transport tools
	const lta::SidednessAgreement sidedness{sidednessPolicy};
	const lta::DirectLightEstimator directLight{&integrand.getScene(), sidedness};
	const lta::TMIS<lta::EMISStyle::Power> mis{};
	const lta::RussianRoulette rr{};
	const lta::SurfaceTracer surfaceTracer{&integrand.getScene()};

	// Common variables
	math::Spectrum pathEnergy(0);
	math::Spectrum pathThroughput(1);
	SurfaceHit X;
	real rrScale = 1.0_r;

	// Reversing the ray for backward tracing
	Ray tracingRay = Ray(ray).reverse();
	tracingRay.setRange(0, std::numeric_limits<real>::max());

	// 0-bounce direct lighting
	{
		PH_SCOPED_TIMER(ZeroBounceDirect);
		if(!directLight.sampleSurfaceEmission<EEmitterFeatureSet::ZeroBounceSample>(
			tracingRay, sampleFlow, &X, &pathEnergy))
		{
			out_estimation[getPathEnergyIndex()] = pathEnergy;
			return;
		}
	}

	BsdfQueryContext bsdfContext{sidednessPolicy};
	bsdfContext.key = BsdfKey::makeRandom();

	// Ray bouncing around the scene (1 ~ N bounces)
	for(uint32 numBounces = 0; numBounces < MAX_RAY_BOUNCES; numBounces++)
	{
		const SurfaceOptics& surfaceOptics = X.getSurfaceOptics();

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
			if(directLight.neeSampleSurfaceEmission(directSample, sampleFlow, &Xe) &&
			   directSample.outputs)
			{
				const auto L = directSample.getTargetToEmit().normalize();
				const SurfaceEmitter& directEmitter = Xe.getSurfaceEmitter();

				BsdfEvalQuery bsdfEval(bsdfContext, X, L, V);
				surfaceOptics.calcBsdf(bsdfEval);
				if(bsdfEval.outputs.isContributable())
				{
					// MIS: NEE + BSDF sample

					real bsdfSamplePdfW = 0.0_r;
					if(useBsdfLightSampling &&
					   Xe.getMetadata().getSurface().isEmissive() &&
					   directEmitter.getFeatureSet().has(EEmitterFeatureSet::BsdfSample))
					{
						BsdfPdfQuery bsdfPdfQuery(bsdfContext, bsdfEval.inputs);
						surfaceOptics.calcBsdfPdf(bsdfPdfQuery);

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

			BsdfSampleQuery bsdfSample(bsdfContext, X, V);
			surfaceOptics.genBsdfSample(bsdfSample, sampleFlow);
			if(!bsdfSample.outputs.isContributable())
			{
				break;
			}

			const math::Vector3R N = X.getShadingNormal();
			const math::Vector3R L = bsdfSample.outputs.getL();

			PH_ASSERT_MSG(L.isFinite(),
				"L = " + L.toString() + ", from " + surfaceOptics.toString());

			pathThroughput *= bsdfSample.outputs.getPdfAppliedBsdfCos();

			// Trace a ray using BSDF's suggestion
			tracingRay.setOrigin(X.getPos());
			tracingRay.setDir(L);
			SurfaceHit nextX;
			Ray boundedRay;
			const bool foundGeometry = surfaceTracer.traceNextSurfaceFrom(
				X,
				tracingRay,
				&nextX,
				&boundedRay);
			if(foundGeometry)
			{
				sidedness.adjustForSidednessAgreement(nextX);
			}

			const bool foundSurface = foundGeometry && sidedness.isSidednessAgreed(nextX, boundedRay.getDir());
			if(useBsdfLightSampling)
			{
				std::optional<real> bsdfSamplePdfW;
				pathEnergy += directLight.accumulateSurfaceEmission<EEmitterFeatureSet::BsdfSample>(
					boundedRay,
					foundSurface ? &nextX : nullptr,
					sampleFlow,
					[&](const SurfaceHit& Xe)
					{
						math::Spectrum radianceLe;
						Xe.getSurfaceEmitter().evalEmittedEnergy(Xe, &radianceLe);
						if(radianceLe.isZero())
						{
							return math::Spectrum(0);
						}

						// TODO: not doing MIS if delta elemental exists is too harsh--we can do regular sample for
						// deltas and MIS for non-deltas

						// No MIS: BSDF sample only
						real misWeighting = 1;

						// MIS: BSDF sample + NEE
						if(useNeeLightSampling)
						{
							// All emitter hits on this segment share the same BSDF PDF
							if(!bsdfSamplePdfW)
							{
								BsdfPdfQuery bsdfPdfQuery(bsdfContext, bsdfSample);
								surfaceOptics.calcBsdfPdf(bsdfPdfQuery);
								bsdfSamplePdfW = bsdfPdfQuery.outputs ? bsdfPdfQuery.outputs.getSampleDirPdfW() : 0;
							}

							// `isNeeSamplable()` is already `true`, but BSDF PDF can still be empty or 0
							// (e.g., sidedness policy or by the distribution itself)
							if(*bsdfSamplePdfW == 0)
							{
								return math::Spectrum(0);
							}

							// `directLightPdfW` can be 0 and MIS weighting still works.
							// No need to test occlusion again here.
							const real directLightPdfW = directLight.calcNeePdfWUnoccluded(X, Xe);
							misWeighting = mis.weight(*bsdfSamplePdfW, directLightPdfW);
						}

						math::Spectrum weight = pathThroughput;
						weight *= misWeighting;

						// Avoid excessive, negative weight and possible NaNs
						rationalClamp(weight);

						return radianceLe * weight;
					});
			}

			if(!foundSurface)
			{
				break;
			}

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

			// Will extend the path, update states for next bounce
			X = nextX;
			bsdfContext.key = bsdfContext.key.makeRandom();
		}
	}// end for each bounces

	PH_ASSERT_MSG(pathThroughput.isFinite() && pathEnergy.isFinite(),
		"pathThroughput = " + pathThroughput.toString() + ", pathEnergy = " + pathEnergy.toString());

	out_estimation[getPathEnergyIndex()] = pathEnergy;
}

void BNEEPTEstimator::rationalClamp(math::Spectrum& value)
{
	// TODO: should negative value be allowed?
	value.safeClampLocal(0.0_r, 1e9_r);
}

}// end namespace ph
