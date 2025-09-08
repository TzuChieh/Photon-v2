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

inline bool DirectLightEstimator::bsdfSampleSurfaceEmission(
	BsdfSampleQuery&                 bsdfSample,
	SampleFlow&                      sampleFlow,
	math::Spectrum* const            out_Le,
	std::optional<SurfaceHit>* const out_X) const
{
	SurfaceHit nextX;
	const bool foundNextX = SurfaceTracer{m_scene}.bsdfSampleNextSurface(bsdfSample, sampleFlow, &nextX);
	if(!bsdfSample.outputs.isContributable())
	{
		return false;
	}

	math::Spectrum Le(0);
	if(foundNextX)
	{
		const SurfaceEmitter& nextEmitter = nextX.getSurfaceEmitter();
		if(nextX.getMetadata().getSurface().isEmissive() &&
		   nextEmitter.getFeatureSet().has(EEmitterFeatureSet::BsdfSample))
		{
			nextEmitter.evalEmittedEnergy(nextX, &Le);
		}
	}

	PH_ASSERT_IN_RANGE(bsdfSample.outputs.getL().lengthSquared(), 0.9_r, 1.1_r);
	
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

	constexpr SurfaceHitReason reason{ESurfaceHitReason::SampledPos};
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
	std::optional<SurfaceHit>* const out_X) const
{
	using MIS = TMIS<EMISStyle::Power>;

	const SurfaceHit&    X = bsdfSample.inputs.getX();
	const math::Vector3R V = X.getIncidentRay().getDir().mul(-1);
	const math::Vector3R N = X.getShadingNormal();
	math::Spectrum sampledLo(0);

	// BSDF sample
	{
		math::Spectrum bsdfLe;
		std::optional<SurfaceHit> nextX;
		if(bsdfSampleSurfaceEmission(bsdfSample, sampleFlow, &bsdfLe, &nextX) &&
		   bsdfSample.outputs.isContributable() &&
		   nextX)
		{
			const SurfaceOptics& optics = X.getSurfaceOptics();

			const auto pdfAppliedBsdfCos = bsdfSample.outputs.getPdfAppliedBsdfCos();

			// If NEE cannot sample the same light from `X` (due to delta BSDF, etc.), then we
			// cannot use MIS weighting to remove NEE contribution as BSDF sampling may not
			// always have an explicit PDF term.
			
			// MIS
			if(isNeeSamplable(X) && nextX->getMetadata().getSurface().isEmissive())
			{
				// No need to test occlusion again as `bsdfSampleSurfaceEmission()` already done that
				const real neePdfW = neeSamplePdfWUnoccluded(X, *nextX);

				BsdfPdfQuery bsdfPdfQuery{bsdfSample.context};
				bsdfPdfQuery.inputs.set(bsdfSample);
				optics.calcBsdfPdf(bsdfPdfQuery);

				// `isNeeSamplable()` is already checked, but BSDF PDF can still be empty or 0
				// (e.g., sidedness policy or by the distribution itself)
				if(bsdfPdfQuery.outputs)
				{
					const real bsdfSamplePdfW = bsdfPdfQuery.outputs.getSampleDirPdfW();
					const real misWeighting   = MIS{}.weight(bsdfSamplePdfW, neePdfW);

					math::Spectrum weight(pdfAppliedBsdfCos * misWeighting);

					// Avoid excessive, negative weight and possible NaNs
					weight.safeClampLocal(0.0_r, 1e9_r);

					sampledLo += bsdfLe * weight;
				}
			}
			// BSDF sample only
			else
			{
				sampledLo += bsdfLe * pdfAppliedBsdfCos;
			}
		}

		// If BSDF sampling failed for whatever reason, we cannot simply return `false`
		// as NEE could still sample a non-zero outgoing energy
		if(out_X) { *out_X = nextX; }
	}

	// NEE
	if(isNeeSamplable(X))
	{
		const DirectLightEstimator estimator{m_scene, bsdfSample.context.sidedness};

		DirectEnergySampleQuery directSample;
		directSample.inputs.set(bsdfSample.inputs.getX());
		if(estimator.neeSampleSurfaceEmission(directSample, sampleFlow) &&
		   directSample.outputs)
		{
			// Always do MIS. If NEE can sample a light from `X`, then BSDF light sample should have
			// no problem doing the same. No need to consider delta light sources as Photon do not
			// have them.

			const SurfaceOptics& optics = X.getSurfaceOptics();

			BsdfEvalQuery bsdfEval{bsdfSample.context};
			bsdfEval.inputs.set(X, directSample.getTargetToEmit().normalize(), V);
			optics.calcBsdf(bsdfEval);
			if(bsdfEval.outputs.isContributable())
			{
				BsdfPdfQuery bsdfPdfQuery{bsdfSample.context};
				bsdfPdfQuery.inputs.set(bsdfEval.inputs);
				optics.calcBsdfPdf(bsdfPdfQuery);
				if(bsdfPdfQuery.outputs)
				{
					const auto L              = bsdfEval.inputs.getL();
					const real neePdfW        = directSample.outputs.getPdfW();
					const real bsdfSamplePdfW = bsdfPdfQuery.outputs.getSampleDirPdfW();
					const real misWeighting   = MIS{}.weight(neePdfW, bsdfSamplePdfW);

					math::Spectrum weight(bsdfEval.outputs.getBsdf() * N.absDot(L) * misWeighting / neePdfW);

					// Avoid excessive, negative weight and possible NaNs
					weight.safeClampLocal(0.0_r, 1e9_r);

					sampledLo += directSample.outputs.getEmittedEnergy() * weight;
				}
			}
		}
	}
	
	if(out_Lo) { *out_Lo = sampledLo; }

	return true;
}

inline real DirectLightEstimator::neeSamplePdfWUnoccluded(
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

inline const Scene& DirectLightEstimator::getScene() const
{
	PH_ASSERT(m_scene);

	return *m_scene;
}

}// end namespace ph::lta
