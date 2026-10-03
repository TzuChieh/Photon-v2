#include "Engine/Core/LTA/DirectLightEstimator.h"
#include "Engine/World/Scene.h"
#include "Engine/Core/Emitter/Emitter.h"
#include "Engine/Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Engine/Core/Emitter/Query/DirectEnergyPdfQuery.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/HitProbe.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/SampleGenerator/SampleFlow.h"
#include "Engine/Core/LTA/lta.h"
#include "Engine/Core/LTA/SurfaceTracer.h"
#include "Engine/Core/LTA/TMIS.h"
#include "Engine/Core/LTA/SurfaceHitRefinery.h"
#include "Engine/Math/TVector3.h"

#include <Common/assertion.h>

#include <algorithm>
#include <limits>

namespace ph::lta
{

inline DirectLightEstimator::DirectLightEstimator(
	const Scene* const scene,
	const SidednessAgreement& defaultSidedness)

	: m_scene(scene)
	, m_defaultSidedness(defaultSidedness)
{
	PH_ASSERT(scene);
}

template<EEmitterFeatureSet FEATURE>
inline bool DirectLightEstimator::sampleSurfaceEmission(
	const Ray& ray,
	SampleFlow& sampleFlow,
	SurfaceHit* const out_X,
	math::Spectrum* const out_Le) const
{
	PH_ASSERT(out_X && out_Le);

	const SurfaceTracer tracer{m_scene};

	Ray boundedRay;
	const bool foundGeometry = tracer.traceNextSurface(ray, out_X, &boundedRay);
	if(foundGeometry)
	{
		m_defaultSidedness.adjustForSidednessAgreement(*out_X);
	}

	const bool foundSurface = foundGeometry && m_defaultSidedness.isSidednessAgreed(*out_X, ray.getDir());
	*out_Le = accumulateSurfaceEmission<FEATURE>(
		boundedRay,
		foundSurface ? out_X : nullptr,
		sampleFlow,
		[](const SurfaceHit& Xe)
		{
			math::Spectrum Le;
			Xe.getSurfaceEmitter().evalEmittedEnergy(Xe, &Le);
			return Le;
		});
	return foundSurface;
}

inline bool DirectLightEstimator::bsdfSampleSurfaceEmission(
	BsdfSampleQuery&                 bsdfSample,
	SampleFlow&                      sampleFlow,
	math::Spectrum* const            out_Le,
	std::optional<SurfaceHit>* const out_X) const
{
	const SurfaceTracer tracer{m_scene};

	Ray sampledRay;
	if(!tracer.doBsdfSample(bsdfSample, sampleFlow, &sampledRay))
	{
		return false;
	}

	const Ray refinedRay = tracer.getRefinedRayOriginatedFrom(bsdfSample.inputs.getX(), sampledRay);
	const DirectLightEstimator estimator{m_scene, bsdfSample.context.sidedness};

	SurfaceHit nextX;
	math::Spectrum Le;
	const bool foundNextX = estimator.sampleSurfaceEmission<EEmitterFeatureSet::BsdfSample>(
		refinedRay, sampleFlow, &nextX, &Le);

	if(out_Le) { *out_Le = Le; }
	if(out_X)  { *out_X = foundNextX ? std::make_optional(nextX) : std::nullopt; }

	return true;
}

inline bool DirectLightEstimator::neeSampleSurfaceEmission(
	DirectEnergySampleQuery&  directSample,
	SampleFlow&               sampleFlow,
	SurfaceHit* const         out_Xe) const
{
	PH_ASSERT(isNeeSamplable(directSample.inputs.getX()));

	const SurfaceHit& X = directSample.inputs.getX();

	HitProbe probe;
	getScene().genDirectSample(directSample, sampleFlow, probe);
	if(!directSample.outputs || !m_defaultSidedness.isSidednessAgreed(X, directSample.getTargetToEmit()))
	{
		return false;
	}

	constexpr SurfaceHitReasons reason{ESurfaceHitReason::SampledPos};
	const SurfaceHit Xe(directSample.outputs.getObservationRay(), probe, reason);
	const auto optVisibilityRay = SurfaceHitRefinery{X}.tryEscape(Xe);
	if(!optVisibilityRay || getScene().isOccluding(*optVisibilityRay))
	{
		return false;
	}

	PH_ASSERT_IN_RANGE(optVisibilityRay->getDir().lengthSquared(), 0.9_r, 1.1_r);
	PH_ASSERT(Xe.getMetadata().getSurface().isEmissive());

	if(out_Xe) { *out_Xe = Xe; }

	return true;
}

inline bool DirectLightEstimator::bsdfSampleSurfacePathWithNee(
	BsdfSampleQuery&                 bsdfSample,
	SampleFlow&                      sampleFlow,
	math::Spectrum* const            out_Lo,
	std::optional<SurfaceHit>* const out_X,
	const real                       nonBlockingSampleProbability) const
{
	using MIS = TMIS<EMISStyle::Power>;

	const SurfaceHit& X = bsdfSample.inputs.getX();
	const math::Vector3R V = X.getIncidentRay().getDir().mul(-1);
	const math::Vector3R N = X.getShadingNormal();
	const bool useNeeLightSampling = isNeeSamplable(X);
	math::Spectrum sampledLo(0);

	// BSDF sample
	{
		const SurfaceTracer tracer{m_scene};

		std::optional<SurfaceHit> nextX;
		Ray sampledRay;
		if(tracer.doBsdfSample(bsdfSample, sampleFlow, &sampledRay))
		{
			const SidednessAgreement& sidedness = bsdfSample.context.sidedness;
			const DirectLightEstimator estimator{m_scene, sidedness};

			SurfaceHit nextHit;
			Ray boundedRay;
			const bool foundGeometry = tracer.traceNextSurfaceFrom(
				X,
				sampledRay,
				&nextHit,
				&boundedRay);
			
			if(foundGeometry)
			{
				sidedness.adjustForSidednessAgreement(nextHit);
				if(sidedness.isSidednessAgreed(nextHit, boundedRay.getDir()))
				{
					nextX = nextHit;
				}
			}

			std::optional<real> bsdfSamplePdfW;
			sampledLo += estimator.accumulateSurfaceEmission<EEmitterFeatureSet::BsdfSample>(
				boundedRay,
				nextX ? &*nextX : nullptr,
				sampleFlow,
				[this, &bsdfSample, &X, &bsdfSamplePdfW, useNeeLightSampling](const SurfaceHit& Xe)
				{
					math::Spectrum bsdfLe;
					Xe.getSurfaceEmitter().evalEmittedEnergy(Xe, &bsdfLe);
					if(bsdfLe.isZero())
					{
						return math::Spectrum(0);
					}

					const auto pdfAppliedBsdfCos = bsdfSample.outputs.getPdfAppliedBsdfCos();

					// If NEE cannot sample the same light from `X` (due to delta BSDF, etc.), then we
					// cannot use MIS weighting to remove NEE contribution as BSDF sampling may not
					// always have an explicit PDF term.

					// MIS
					if(useNeeLightSampling)
					{
						// Query and cache `bsdfSamplePdfW` when an emitter contributes to the sampled segment.
						if(!bsdfSamplePdfW)
						{
							BsdfPdfQuery bsdfPdfQuery{bsdfSample.context, bsdfSample};
							X.getSurfaceOptics().calcBsdfPdf(bsdfPdfQuery);
							bsdfSamplePdfW = bsdfPdfQuery.outputs ? bsdfPdfQuery.outputs.getSampleDirPdfW() : 0;
						}

						// `isNeeSamplable()` is already checked, but BSDF PDF can still be empty or 0
						// (e.g., sidedness policy or by the distribution itself)
						if(*bsdfSamplePdfW == 0)
						{
							return math::Spectrum(0);
						}

						// No need to test occlusion again as `boundedRay` is already the unoccluded segment.
						// `neePdfW` can be 0 and this is fine--MIS weighting still works.
						const real neePdfW = calcNeePdfWUnoccluded(X, Xe);
						const real misWeighting = MIS{}.weight(*bsdfSamplePdfW, neePdfW);
						math::Spectrum weight(pdfAppliedBsdfCos * misWeighting);

						// Avoid excessive, negative weight and possible NaNs
						weight.safeClampLocal(0.0_r, 1e9_r);

						return bsdfLe * weight;
					}
					// BSDF sample only
					else
					{
						return bsdfLe * pdfAppliedBsdfCos;
					}
				},
				nonBlockingSampleProbability);// end `accumulateSurfaceEmission`
		}

		// If BSDF sampling failed for whatever reason, we cannot simply return `false`
		// as NEE could still sample a non-zero outgoing energy
		if(out_X) { *out_X = nextX; }
	}

	// NEE
	if(useNeeLightSampling)
	{
		const DirectLightEstimator estimator{m_scene, bsdfSample.context.sidedness};

		DirectEnergySampleQuery directSample;
		directSample.inputs.set(bsdfSample.inputs.getX());

		SurfaceHit Xe;
		if(estimator.neeSampleSurfaceEmission(directSample, sampleFlow, &Xe) &&
		   directSample.outputs)
		{
			// MIS must only account for enabled techniques. NEE can sample emitters whose
			// BSDF sampling has been disabled for, even when their shapes are present.

			const SurfaceOptics& optics = X.getSurfaceOptics();

			BsdfEvalQuery bsdfEval{bsdfSample.context, X, directSample.getTargetToEmit().normalize(), V};
			optics.calcBsdf(bsdfEval);
			if(bsdfEval.outputs.isContributable())
			{
				real bsdfSamplePdfW = 0.0_r;
				if(Xe.getSurfaceEmitter().getFeatureSet().has(EEmitterFeatureSet::BsdfSample))
				{
					BsdfPdfQuery bsdfPdfQuery{bsdfSample.context, bsdfEval.inputs};
					optics.calcBsdfPdf(bsdfPdfQuery);
					bsdfSamplePdfW = bsdfPdfQuery.outputs ? bsdfPdfQuery.outputs.getSampleDirPdfW() : 0.0_r;
				}

				// MIS: NEE + BSDF sample
				const auto L = bsdfEval.inputs.getL();
				const real neePdfW = directSample.outputs.getPdfW();
				const real misWeighting = MIS{}.weight(neePdfW, bsdfSamplePdfW);

				math::Spectrum weight(bsdfEval.outputs.getBsdf() * N.absDot(L) * misWeighting / neePdfW);

				// Avoid excessive, negative weight and possible NaNs
				weight.safeClampLocal(0.0_r, 1e9_r);

				sampledLo += directSample.outputs.getEmittedEnergy() * weight;
			}
		}
	}

	if(out_Lo) { *out_Lo = sampledLo; }

	return true;
}

inline real DirectLightEstimator::calcNeePdfWUnoccluded(
	const SurfaceHit&     X,
	const SurfaceHit&     Xe) const
{
	PH_ASSERT(isNeeSamplable(X));
	PH_ASSERT(Xe.getMetadata().getSurface().isEmissive());

	DirectEnergyPdfQuery pdfQuery;
	pdfQuery.inputs.set(X, Xe);
	getScene().calcDirectPdf(pdfQuery);
	return pdfQuery.outputs ? pdfQuery.outputs.getPdfW() : 0;
}

inline bool DirectLightEstimator::isNeeSamplable(const SurfaceHit& X) const
{
	const SurfaceOptics& optics = X.getSurfaceOptics();
	return optics.getAllPhenomena().hasNone(ESurfacePhenomenon::Delta);
}

template<EEmitterFeatureSet FEATURE, typename EnergyFunc>
inline math::Spectrum DirectLightEstimator::accumulateSurfaceEmission(
	const Ray& ray,
	const SurfaceHit* const endpoint,
	SampleFlow& sampleFlow,
	EnergyFunc&& energyFunc,
	const real nonBlockingSampleProbability) const
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(nonBlockingSampleProbability, 0, 1);

	math::Spectrum energy(0);
	if(nonBlockingSampleProbability > 0 &&
	   (nonBlockingSampleProbability == 1 || sampleFlow.pick(nonBlockingSampleProbability)))
	{
		SurfaceTracer{m_scene}.forEachNonBlockingEmitterHit<FEATURE>(
			ray,
			[this, &ray, &energy, &energyFunc](SurfaceHit& Xe)
			{
				m_defaultSidedness.adjustForSidednessAgreement(Xe);
				if(m_defaultSidedness.isSidednessAgreed(Xe, ray.getDir()))
				{
					energy += energyFunc(Xe);
				}
			});

		// Compensate the roulette after contribution weighting and clamping
		energy *= 1 / nonBlockingSampleProbability;
	}

	if(endpoint &&
	   endpoint->getMetadata().getSurface().isEmissive() &&
	   endpoint->getSurfaceEmitter().getFeatureSet().hasAny(FEATURE))
	{
		energy += energyFunc(*endpoint);
	}

	return energy;
}

inline const Scene& DirectLightEstimator::getScene() const
{
	PH_ASSERT(m_scene);

	return *m_scene;
}

}// end namespace ph::lta
