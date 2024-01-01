#include "Core/LTA/TDirectLightEstimator.h"
#include "World/Scene.h"
#include "Core/Emitter/Emitter.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Core/Ray.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/LTA/lta.h"
#include "Core/LTA/SurfaceTracer.h"
#include "Core/LTA/TMis.h"
#include "Math/TVector3.h"

#include <Common/assertion.h>

#include <algorithm>

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

	const Emitter* emitter = getEmitter(Xe);
	if(!emitter)
	{
		return false;
	}

	math::Spectrum Le;
	emitter->evalEmittedRadiance(Xe, &Le);
	
	if(out_L)              { *out_L = bsdfSample.outputs.L; }
	if(out_pdfAppliedBsdf) { *out_pdfAppliedBsdf = bsdfSample.outputs.pdfAppliedBsdf; }
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

	DirectEnergySampleQuery directLightSample;
	directLightSample.in.set(X.getPosition());
	getScene().genDirectSample(directLightSample, sampleFlow);
	if(!directLightSample.out)
	{
		return false;
	}

	// Before visibility test, make sure the vector to light is long enough to avoid self-intersection
	// (at least 3 deltas, 2 for ray endpoints and 1 for ray body, HACK). Also checks sidedness
	// agreement between real geometry and shading normal.
	const auto toLightVec = directLightSample.out.emitPos - directLightSample.in.targetPos;
	if(toLightVec.lengthSquared() <= math::squared(self_intersect_delta * 3) ||
	   !SidednessAgreement(POLICY).isSidednessAgreed(X, toLightVec))
	{
		return false;
	}

	// If surface hit info is requested, a potentially more expensive intersection test is needed
	// for the visibility test
	if(out_Xe)
	{
		const Ray visibilityRay(
			X.getPosition(), 
			toLightVec.normalize(), 
			self_intersect_delta,
			toLightVec.length() * 2,// HACK: to ensure an intersection
			X.getIncidentRay().getTime());

		SurfaceHit Xe;
		if(!SurfaceTracer(m_scene).traceNextSurface(visibilityRay, SidednessAgreement(POLICY), &Xe) ||
		   Xe.getDetail().getPrimitive() != directLightSample.out.srcPrimitive)
		{
			return false;
		}

		*out_Xe = Xe;
		if(out_L) { *out_L = visibilityRay.getDirection(); }
	}
	// Occlusion test
	else
	{
		const Ray visibilityRay(
			X.getPosition(), 
			toLightVec.normalize(), 
			self_intersect_delta,
			toLightVec.length() - self_intersect_delta * 2,
			X.getIncidentRay().getTime());

		if(getScene().isOccluding(visibilityRay))
		{
			return false;
		}

		if(out_L) { *out_L = visibilityRay.getDirection(); }
	}

	if(out_pdfW) { *out_pdfW = directLightSample.out.pdfW; }
	if(out_Le)   { *out_Le = directLightSample.out.radianceLe; }

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
			const SurfaceOptics* optics = getSurfaceOptics(X);
			PH_ASSERT(optics);

			// If NEE cannot sample the same light from `X` (due to delta BSDF, etc.), then we
			// cannot use MIS weighting to remove NEE contribution as BSDF sampling may not
			// always have an explicit PDF term.
			
			// MIS
			if(isNeeSamplable(X))
			{
				// No need to test occlusion again as `bsdfSample()` already done that
				const real neePdfW = neeSamplePdfWUnoccluded(X, Xe);

				BsdfPdfQuery bsdfPdfQuery{BsdfQueryContext(POLICY)};
				bsdfPdfQuery.inputs.set(X, L, V);
				optics->calcBsdfSamplePdfW(bsdfPdfQuery);

				const real bsdfSamplePdfW = bsdfPdfQuery.outputs.sampleDirPdfW;
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

			const SurfaceOptics* optics = getSurfaceOptics(X);
			PH_ASSERT(optics);

			BsdfEvalQuery bsdfEval{BsdfQueryContext(POLICY)};
			bsdfEval.inputs.set(X, L, V);

			optics->calcBsdf(bsdfEval);
			if(bsdfEval.outputs.isGood())
			{
				BsdfPdfQuery bsdfPdfQuery{BsdfQueryContext(POLICY)};
				bsdfPdfQuery.inputs.set(bsdfEval);
				optics->calcBsdfSamplePdfW(bsdfPdfQuery);

				const real bsdfSamplePdfW = bsdfPdfQuery.outputs.sampleDirPdfW;
				const real misWeighting   = MIS{}.weight(neePdfW, bsdfSamplePdfW);

				math::Spectrum weight(bsdfEval.outputs.bsdf * N.absDot(L) * misWeighting / neePdfW);

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

	return getScene().calcDirectPdfW(Xe, X.getPosition());
}

template<ESidednessPolicy POLICY>
inline bool TDirectLightEstimator<POLICY>::isNeeSamplable(const SurfaceHit& X) const
{
	const SurfaceOptics* optics = getSurfaceOptics(X);
	return optics && optics->getAllPhenomena().hasNone(
		{ESurfacePhenomenon::DeltaReflection, ESurfacePhenomenon::DeltaTransmission});
}

template<ESidednessPolicy POLICY>
inline const Scene& TDirectLightEstimator<POLICY>::getScene() const
{
	PH_ASSERT(m_scene);

	return *m_scene;
}

template<ESidednessPolicy POLICY>
inline const Primitive& TDirectLightEstimator<POLICY>::getPrimitive(const SurfaceHit& X)
{
	// Does not make sense to call this method if `X` hits nothing
	PH_ASSERT(X.getDetail().getPrimitive());

	return *(X.getDetail().getPrimitive());
}

template<ESidednessPolicy POLICY>
inline const SurfaceOptics* TDirectLightEstimator<POLICY>::getSurfaceOptics(const SurfaceHit& X)
{
	const PrimitiveMetadata* meta = getPrimitive(X).getMetadata();
	return meta ? meta->getSurface().getOptics() : nullptr;
}

template<ESidednessPolicy POLICY>
inline const Emitter* TDirectLightEstimator<POLICY>::getEmitter(const SurfaceHit& X)
{
	const PrimitiveMetadata* meta = getPrimitive(X).getMetadata();
	return meta ? meta->getSurface().getEmitter() : nullptr;
}

}// end namespace ph::lta
