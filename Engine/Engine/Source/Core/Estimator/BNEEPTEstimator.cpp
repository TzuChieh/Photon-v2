#include "Core/Estimator/BNEEPTEstimator.h"
#include "Core/Ray.h"
#include "World/Scene.h"
#include "Math/TVector3.h"
#include "Core/HitDetail.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/math.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Math/Color/Spectrum.h"
#include "Core/LTA/TMis.h"
#include "Core/LTA/TDirectLightEstimator.h"
#include "Core/LTA/RussianRoulette.h"
#include "Core/Estimator/Integrand.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/stats.h>

#define RAY_DELTA_DIST 0.0001f
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

	const Scene&    scene    = integrand.getScene();
	const Receiver& receiver = integrand.getReceiver();

	const lta::TDirectLightEstimator<lta::ESidednessPolicy::Strict> directLight{&scene};
	const lta::TMis<lta::EMisStyle::Power> mis{};
	const lta::RussianRoulette rr{};

	// common variables
	math::Spectrum accuRadiance(0);
	math::Spectrum accuLiWeight(1);
	HitProbe       hitProbe;
	SurfaceHit     surfaceHit;
	math::Vector3R V;

	// reversing the ray for backward tracing
	Ray tracingRay = Ray(ray).reverse();
	tracingRay.setMinT(0.0001_r);// HACK: hard-coded number
	tracingRay.setMaxT(std::numeric_limits<real>::max());

	if(!scene.isIntersecting(tracingRay, &hitProbe))
	{
		out_estimation[m_estimationIndex] = accuRadiance;
		return;
	}

	// 0-bounce direct lighting
	{
		PH_SCOPED_TIMER(ZeroBounceDirect);

		surfaceHit = SurfaceHit(tracingRay, hitProbe);

		// sidedness agreement between real geometry and shading normal
		V = tracingRay.getDirection().mul(-1.0_r);
		if(surfaceHit.getGeometryNormal().dot(V) * surfaceHit.getShadingNormal().dot(V) <= 0.0_r)
		{
			out_estimation[m_estimationIndex] = accuRadiance;
			return;
		}

		const PrimitiveMetadata* metadata        = surfaceHit.getDetail().getPrimitive()->getMetadata();
		const SurfaceBehavior&   surfaceBehavior = metadata->getSurface();
		if(surfaceBehavior.getEmitter())
		{
			math::Spectrum radianceLi;
			surfaceBehavior.getEmitter()->evalEmittedRadiance(surfaceHit, &radianceLi);
			accuRadiance.addLocal(radianceLi);
		}
	}

	// ray bouncing around the scene (1 ~ N bounces)
	for(uint32 numBounces = 0; numBounces < MAX_RAY_BOUNCES; numBounces++)
	{
		// FIXME: too hacky
		bool canDoNEE = true;
		{
			const PrimitiveMetadata* me = surfaceHit.getDetail().getPrimitive()->getMetadata();
			const SurfaceOptics* op = me->getSurface().getOptics();
			if(op->getAllPhenomena().hasAny({ESurfacePhenomenon::DeltaReflection, ESurfacePhenomenon::DeltaTransmission}))
			{
				canDoNEE = false;
			}
		}

		// direct light sample
		{
			PH_SCOPED_TIMER(DirectLightSampling);

			math::Vector3R L;
			real           directPdfW;
			math::Spectrum emittedRadiance;

			if(canDoNEE && directLight.neeSampleEmission(
				surfaceHit, sampleFlow,
				&L, &directPdfW, &emittedRadiance))
			{
				const PrimitiveMetadata* metadata        = surfaceHit.getDetail().getPrimitive()->getMetadata();
				const SurfaceBehavior&   surfaceBehavior = metadata->getSurface();

				BsdfEvalQuery bsdfEval;
				bsdfEval.inputs.set(surfaceHit, L, V);

				surfaceBehavior.getOptics()->calcBsdf(bsdfEval);
				if(bsdfEval.outputs.isMeasurable())
				{
					BsdfPdfQuery bsdfPdfQuery;
					bsdfPdfQuery.inputs.set(bsdfEval);
					surfaceBehavior.getOptics()->calcBsdfSamplePdfW(bsdfPdfQuery);

					const real bsdfSamplePdfW = bsdfPdfQuery.outputs.sampleDirPdfW;
					const real misWeighting = mis.weight(directPdfW, bsdfSamplePdfW);
					const math::Vector3R N = surfaceHit.getShadingNormal();

					math::Spectrum weight;
					weight = bsdfEval.outputs.bsdf.mul(N.absDot(L));
					weight.mulLocal(accuLiWeight).mulLocal(misWeighting / directPdfW);

					// avoid excessive, negative weight and possible NaNs
					rationalClamp(weight);

					accuRadiance.addLocal(emittedRadiance.mul(weight));
				}
			}
		}// end direct light sample

		// BSDF sample + indirect light sample
		{
			PH_SCOPED_TIMER(BSDFAndIndirectLightSampling);

			const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
			const SurfaceBehavior*   surfaceBehavior = &(metadata->getSurface());

			BsdfSampleQuery bsdfSample;
			bsdfSample.inputs.set(surfaceHit, V);

			surfaceBehavior->getOptics()->calcBsdfSample(bsdfSample, sampleFlow);

			const math::Vector3R N = surfaceHit.getShadingNormal();
			const math::Vector3R L = bsdfSample.outputs.L;

			// blackness check & sidedness agreement between real geometry and shading normal
			//
			if(!bsdfSample.outputs.isMeasurable() ||
			   surfaceHit.getGeometryNormal().dot(L) * surfaceHit.getShadingNormal().dot(L) <= 0.0_r)
			{
				break;
			}

			// DEBUG
			if(!L.isFinite())
			{
				std::cerr << surfaceBehavior->getOptics()->toString() << std::endl;
				return;
			}

			PH_ASSERT(L.isFinite());

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

			// trace a ray using BSDF's suggestion
			//
			tracingRay.setOrigin(surfaceHit.getPosition());
			tracingRay.setDirection(L);
			if(!scene.isIntersecting(tracingRay, &hitProbe))
			{
				break;
			}
			const SurfaceHit Xe = SurfaceHit(tracingRay, hitProbe);
			if(Xe.getGeometryNormal().dot(L) * Xe.getShadingNormal().dot(L) <= 0.0_r)
			{
				break;
			}

			// FIXME: this is dangerous... setting metadata to new hit point
			metadata = Xe.getDetail().getPrimitive()->getMetadata();

			const Emitter* emitter = metadata->getSurface().getEmitter();
			if(emitter)
			{
				math::Spectrum radianceLe;
				emitter->evalEmittedRadiance(Xe, &radianceLe);

				// TODO: not doing MIS if delta elemental exists is too harsh--we can do regular sample for
				// deltas and MIS for non-deltas

				// do MIS
				if(canDoNEE && !radianceLe.isZero())
				{
					// TODO: <directLightPdfW> might be 0, should we stop using MIS if one of two 
					// sampling techniques has failed?
					// <bsdfSamplePdfW> can also be 0 for delta distributions
					const real directLightPdfW = directLight.neeSamplePdfWUnoccluded(
						surfaceHit, Xe);

					BsdfPdfQuery bsdfPdfQuery;
					bsdfPdfQuery.inputs.set(bsdfSample);
					surfaceBehavior->getOptics()->calcBsdfSamplePdfW(bsdfPdfQuery);

					const real bsdfSamplePdfW = bsdfPdfQuery.outputs.sampleDirPdfW;
					if(bsdfSamplePdfW > 0)
					{
						const real misWeighting = mis.weight(bsdfSamplePdfW, directLightPdfW);

						math::Spectrum weight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));
						weight.mulLocal(accuLiWeight).mulLocal(misWeighting);

						// Avoid excessive, negative weight and possible NaNs
						rationalClamp(weight);

						accuRadiance.addLocal(radianceLe.mulLocal(weight));
					}
				}
				// not do MIS
				else
				{
					math::Spectrum weight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));
					weight.mulLocal(accuLiWeight);

					accuRadiance.addLocal(radianceLe.mulLocal(weight));
				}
			}

			const math::Spectrum currentLiWeight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));
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

			// avoid excessive, negative weight and possible NaNs
			//
			rationalClamp(accuLiWeight);

			if(accuLiWeight.isZero())
			{
				break;
			}

			V = tracingRay.getDirection().mul(-1.0_r);
			PH_ASSERT_MSG(V.isFinite(), V.toString());

			surfaceHit = Xe;
		}
	}// end for each bounces

	out_estimation[m_estimationIndex] = accuRadiance;
}

void BNEEPTEstimator::rationalClamp(math::Spectrum& value)
{
	// TODO: should negative value be allowed?
	value.clampLocal(0.0_r, 1e9_r);
}

}// end namespace ph
