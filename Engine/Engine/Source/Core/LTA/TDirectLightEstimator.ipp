#include "Core/LTA/TDirectLightEstimator.h"
#include "World/Scene.h"
#include "Core/Emitter/Emitter.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/Emitter/Query/DirectEnergySamplePdfQuery.h"
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
#include "Core/LTA/TMis.h"
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
	const SurfaceHit&         X,
	SampleFlow&               sampleFlow,
	math::Vector3R* const     out_L,
	math::Spectrum* const     out_pdfAppliedBsdf,
	math::Spectrum* const     out_Le,
	SurfaceHit* const         out_Xe) const
{
	const math::Vector3R V = X.getIncidentRay().getDirection().mul(-1);

	BsdfSampleQuery bsdfSample{BsdfQueryContext(POLICY)};
	bsdfSample.inputs.set(X, V);

	SurfaceHit Xe;
	if(!SurfaceTracer(m_scene).bsdfSampleNextSurface(bsdfSample, sampleFlow, &Xe))
	{
		return false;
	}

	const Emitter* emitter = Xe.getSurfaceEmitter();
	if(!emitter)
	{
		return false;
	}

	math::Spectrum Le;
	emitter->evalEmittedRadiance(Xe, &Le);

	PH_ASSERT_IN_RANGE(bsdfSample.outputs.getL().lengthSquared(), 0.9_r, 1.1_r);
	
	if(out_L)              { *out_L = bsdfSample.outputs.getL(); }
	if(out_pdfAppliedBsdf) { *out_pdfAppliedBsdf = bsdfSample.outputs.getPdfAppliedBsdf(); }
	if(out_Le)             { *out_Le = Le; }
	if(out_Xe)             { *out_Xe = Xe; }

	return true;
}

template<ESidednessPolicy POLICY>
inline bool TDirectLightEstimator<POLICY>::neeSampleEmission(
	const SurfaceHit&         X,
	SampleFlow&               sampleFlow,
	math::Vector3R* const     out_L,
	real* const               out_pdfW,
	math::Spectrum* const     out_Le,
	SurfaceHit* const         out_Xe) const
{
	PH_ASSERT(isNeeSamplable(X));

	const SidednessAgreement sidedness{POLICY};

	DirectEnergySampleQuery directSample;
	directSample.inputs.set(X);

	HitProbe probe;
	getScene().genDirectSample(directSample, sampleFlow, probe);
	if(!directSample.outputs)
	{
		return false;
	}

	const auto toLightVec = directSample.outputs.getEmitPos() - directSample.inputs.getTargetPos();
	if(!sidedness.isSidednessAgreed(X, toLightVec))
	{
		return false;
	}

	constexpr SurfaceHitReason reason{ESurfaceHitReason::SampledPos};
	const SurfaceHit Xe(directSample.outputs.getObservationRay(), probe, reason);
	const std::optional<Ray> visibilityRay = SurfaceHitRefinery{X}.tryEscape(Xe);
	if(!visibilityRay || getScene().isOccluding(*visibilityRay))
	{
		return false;
	}

	PH_ASSERT_IN_RANGE(visibilityRay->getDirection().lengthSquared(), 0.9_r, 1.1_r);

	if(out_Xe)   { *out_Xe = Xe; }
	if(out_L)    { *out_L = visibilityRay->getDirection(); }
	if(out_pdfW) { *out_pdfW = directSample.outputs.getPdfW(); }
	if(out_Le)   { *out_Le = directSample.outputs.getEmittedEnergy(); }

	return true;
}

template<ESidednessPolicy POLICY>
inline bool TDirectLightEstimator<POLICY>::bsdfSampleOutgoingWithNee(
	const SurfaceHit&         X,
	SampleFlow&               sampleFlow,
	math::Spectrum* const     out_Lo,
	math::Vector3R* const     out_L,
	math::Spectrum* const     out_pdfAppliedBsdf,
	SurfaceHit* const         out_Xe) const
{
	using MIS = TMis<EMisStyle::Power>;

	const math::Vector3R V = X.getIncidentRay().getDirection().mul(-1);
	const math::Vector3R N = X.getShadingNormal();
	math::Spectrum sampledLo(0);

	// BSDF sample
	{
		math::Vector3R L;
		math::Spectrum pdfAppliedBsdf;
		math::Spectrum bsdfLe;
		SurfaceHit Xe;
		if(bsdfSampleEmission(X, sampleFlow, &L, &pdfAppliedBsdf, &bsdfLe, &Xe))
		{
			const SurfaceOptics* optics = X.getSurfaceOptics();
			PH_ASSERT(optics);

			// If NEE cannot sample the same light from `X` (due to delta BSDF, etc.), then we
			// cannot use MIS weighting to remove NEE contribution as BSDF sampling may not
			// always have an explicit PDF term.
			
			// MIS
			if(isNeeSamplable(X))
			{
				// No need to test occlusion again as `bsdfSampleEmission()` already done that
				const real neePdfW = neeSamplePdfWUnoccluded(X, Xe);

				BsdfPdfQuery bsdfPdfQuery{BsdfQueryContext(POLICY)};
				bsdfPdfQuery.inputs.set(X, L, V);
				optics->calcBsdfSamplePdfW(bsdfPdfQuery);

				// `isNeeSamplable()` is already checked, but `bsdfSamplePdfW` can still be 0 (e.g.,
				// sidedness policy or by the distribution itself).
				const real bsdfSamplePdfW = bsdfPdfQuery.outputs.getSampleDirPdfW();
				if(bsdfSamplePdfW > 0)
				{
					const real misWeighting = MIS{}.weight(bsdfSamplePdfW, neePdfW);

					math::Spectrum weight(pdfAppliedBsdf * N.absDot(L) * misWeighting);

					// Avoid excessive, negative weight and possible NaNs
					weight.clampLocal(0.0_r, 1e9_r);

					sampledLo += bsdfLe * weight;
				}
			}
			// BSDF sample only
			else
			{
				sampledLo += bsdfLe * pdfAppliedBsdf * N.absDot(L);
			}

			PH_ASSERT_IN_RANGE(L.lengthSquared(), 0.9_r, 1.1_r);

			if(out_L)              { *out_L = L; }
			if(out_pdfAppliedBsdf) { *out_pdfAppliedBsdf = pdfAppliedBsdf; }
			if(out_Xe)             { *out_Xe = Xe; }
		}
		else
		{
			return false;
		}
	}

	// NEE
	if(isNeeSamplable(X))
	{
		math::Vector3R L;
		math::Spectrum neeLe;
		real neePdfW;
		if(neeSampleEmission(X, sampleFlow, &L, &neePdfW, &neeLe))
		{
			// Always do MIS. If NEE can sample a light from `X`, then BSDF light sample should have
			// no problem doing the same. No need to consider delta light sources as Photon do not
			// have them.

			const SurfaceOptics* optics = X.getSurfaceOptics();
			PH_ASSERT(optics);

			BsdfEvalQuery bsdfEval{BsdfQueryContext(POLICY)};
			bsdfEval.inputs.set(X, L, V);

			optics->calcBsdf(bsdfEval);
			if(bsdfEval.outputs.isMeasurable())
			{
				BsdfPdfQuery bsdfPdfQuery{BsdfQueryContext(POLICY)};
				bsdfPdfQuery.inputs.set(bsdfEval.inputs);
				optics->calcBsdfSamplePdfW(bsdfPdfQuery);

				const real bsdfSamplePdfW = bsdfPdfQuery.outputs.getSampleDirPdfW();
				const real misWeighting   = MIS{}.weight(neePdfW, bsdfSamplePdfW);

				math::Spectrum weight(bsdfEval.outputs.getBsdf() * N.absDot(L) * misWeighting / neePdfW);

				// Avoid excessive, negative weight and possible NaNs
				weight.clampLocal(0.0_r, 1e9_r);

				sampledLo += neeLe * weight;
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

	DirectEnergySamplePdfQuery pdfQuery;
	pdfQuery.inputs.set(X, Xe);

	HitProbe probe;
	getScene().calcDirectSamplePdfW(pdfQuery, probe);
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
