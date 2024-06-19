#include "Core/LTA/TDirectLightEstimator.h"
#include "World/Scene.h"
#include "Core/Emitter/Emitter.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/Emitter/Query/DirectEnergyPdfQuery.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Core/Ray.h"
#include "Core/LTA/lta.h"
#include "Core/LTA/SurfaceTracer.h"
#include "Core/LTA/TMIS.h"
#include "Core/LTA/SurfaceHitRefinery.h"
#include "Math/TVector3.h"

#include <Common/assertion.h>

#include <algorithm>
#include <limits>

namespace ph::lta
{

template<ESidednessPolicy POLICY>
inline TDirectLightEstimator<POLICY>::TDirectLightEstimator(const Scene* const scene)
	: m_scene(scene)
{
	PH_ASSERT(scene);
}

template<ESidednessPolicy POLICY>
inline bool TDirectLightEstimator<POLICY>::bsdfSampleEmission(
	BsdfSampleQuery&                 bsdfSample,
	SampleFlow&                      sampleFlow,
	math::Spectrum* const            out_Le,
	std::optional<SurfaceHit>* const out_X) const
{
	SurfaceHit nextX;
	const bool foundNextX = SurfaceTracer{m_scene}.bsdfSampleNextSurface(bsdfSample, sampleFlow, &nextX);
	if(!bsdfSample.outputs.isMeasurable())
	{
		return false;
	}

	math::Spectrum Le(0);
	if(foundNextX && nextX.getSurfaceEmitter())
	{
		nextX.getSurfaceEmitter()->evalEmittedRadiance(nextX, &Le);
	}

	PH_ASSERT_IN_RANGE(bsdfSample.outputs.getL().lengthSquared(), 0.9_r, 1.1_r);
	
	if(out_Le) { *out_Le = Le; }
	if(out_X)  { *out_X = foundNextX ? std::make_optional(nextX) : std::nullopt; }

	return true;
}

template<ESidednessPolicy POLICY>
inline bool TDirectLightEstimator<POLICY>::neeSampleEmission(
	DirectEnergySampleQuery&  directSample,
	SampleFlow&               sampleFlow,
	SurfaceHit* const         out_Xe) const
{
	PH_ASSERT(isNeeSamplable(directSample.inputs.getX()));

	const SidednessAgreement sidedness{POLICY};
	const SurfaceHit& X = directSample.inputs.getX();

	HitProbe probe;
	getScene().genDirectSample(directSample, sampleFlow, probe);
	if(!directSample.outputs || !sidedness.isSidednessAgreed(X, directSample.getTargetToEmit()))
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
	PH_ASSERT(Xe.getSurfaceEmitter());

	if(out_Xe) { *out_Xe = Xe; }

	return true;
}

template<ESidednessPolicy POLICY>
inline bool TDirectLightEstimator<POLICY>::bsdfSamplePathWithNee(
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
		if(bsdfSampleEmission(bsdfSample, sampleFlow, &bsdfLe, &nextX) &&
		   bsdfSample.outputs.isMeasurable() &&
		   nextX)
		{
			const SurfaceOptics* optics = X.getSurfaceOptics();
			PH_ASSERT(optics);

			const auto pdfAppliedBsdfCos = bsdfSample.outputs.getPdfAppliedBsdfCos();

			// If NEE cannot sample the same light from `X` (due to delta BSDF, etc.), then we
			// cannot use MIS weighting to remove NEE contribution as BSDF sampling may not
			// always have an explicit PDF term.
			
			// MIS
			if(isNeeSamplable(X) && nextX->getSurfaceEmitter())
			{
				// No need to test occlusion again as `bsdfSampleEmission()` already done that
				const real neePdfW = neeSamplePdfWUnoccluded(X, *nextX);

				BsdfPdfQuery bsdfPdfQuery{bsdfSample.context};
				bsdfPdfQuery.inputs.set(bsdfSample);
				optics->calcBsdfPdf(bsdfPdfQuery);

				// `isNeeSamplable()` is already checked, but `bsdfSamplePdfW` can still be 0 (e.g.,
				// sidedness policy or by the distribution itself).
				const real bsdfSamplePdfW = bsdfPdfQuery.outputs.getSampleDirPdfW();
				if(bsdfSamplePdfW > 0)
				{
					const real misWeighting = MIS{}.weight(bsdfSamplePdfW, neePdfW);
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
		DirectEnergySampleQuery directSample;
		directSample.inputs.set(bsdfSample.inputs.getX());
		if(neeSampleEmission(directSample, sampleFlow) && 
		   directSample.outputs)
		{
			// Always do MIS. If NEE can sample a light from `X`, then BSDF light sample should have
			// no problem doing the same. No need to consider delta light sources as Photon do not
			// have them.

			const SurfaceOptics* optics = X.getSurfaceOptics();
			PH_ASSERT(optics);

			BsdfEvalQuery bsdfEval{bsdfSample.context};
			bsdfEval.inputs.set(X, directSample.getTargetToEmit().normalize(), V);
			optics->calcBsdf(bsdfEval);

			if(bsdfEval.outputs.isMeasurable())
			{
				BsdfPdfQuery bsdfPdfQuery{bsdfSample.context};
				bsdfPdfQuery.inputs.set(bsdfEval.inputs);
				optics->calcBsdfPdf(bsdfPdfQuery);

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
	
	if(out_Lo) { *out_Lo = sampledLo; }

	return true;
}

template<ESidednessPolicy POLICY>
inline real TDirectLightEstimator<POLICY>::neeSamplePdfWUnoccluded(
	const SurfaceHit&     X,
	const SurfaceHit&     Xe) const
{
	PH_ASSERT(isNeeSamplable(X));
	PH_ASSERT(Xe.getSurfaceEmitter());

	DirectEnergyPdfQuery pdfQuery;
	pdfQuery.inputs.set(X, Xe);
	getScene().calcDirectPdf(pdfQuery);
	return pdfQuery.outputs ? pdfQuery.outputs.getPdfW() : 0;
}

template<ESidednessPolicy POLICY>
inline bool TDirectLightEstimator<POLICY>::isNeeSamplable(const SurfaceHit& X) const
{
	const SurfaceOptics* optics = X.getSurfaceOptics();
	return optics && optics->getAllPhenomena().hasNone(DELTA_SURFACE_PHENOMENA);
}

template<ESidednessPolicy POLICY>
inline const Scene& TDirectLightEstimator<POLICY>::getScene() const
{
	PH_ASSERT(m_scene);

	return *m_scene;
}

}// end namespace ph::lta
