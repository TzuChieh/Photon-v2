#include "Core/LTABuildingBlock/TDirectLightEstimator.h"
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
#include "Core/LTABuildingBlock/SurfaceTracer.h"

#include <Common/assertion.h>

#include <algorithm>

namespace ph
{

namespace
{
	// FIXME: hardcoded number
	constexpr real DL_RAY_DELTA_DIST = 0.0001_r;
}

template<ESidednessPolicy POLICY>
inline TDirectLightEstimator<POLICY>::TDirectLightEstimator(const Scene* const scene) : 
	m_scene(scene)
{
	PH_ASSERT(scene);
}

template<ESidednessPolicy POLICY>
inline bool TDirectLightEstimator<POLICY>::bsdfSample(
	const SurfaceHit&         X,
	const math::Vector3R&     V,
	const Time&               time,
	SampleFlow&               sampleFlow,
	math::Vector3R* const     out_L,
	real* const               out_pdfW,
	math::Spectrum* const     out_radianceLe,
	SurfaceHit* const         out_Xe) const
{
	SurfaceTracer tracer(m_scene);

	BsdfSampleQuery sampleQuery;
	sampleQuery.context.sidedness = SidednessAgreement(POLICY);
	sampleQuery.inputs.set(X, V);

	SurfaceHit Xe;
	if(!tracer.bsdfSampleNextSurface(sampleQuery, sampleFlow, &Xe))
	{
		return false;
	}

	const Emitter* emitter = getEmitter(Xe);
	if(!emitter)
	{
		return false;
	}

	BsdfPdfQuery pdfQuery;
	pdfQuery.context.sidedness = SidednessAgreement(POLICY);
	pdfQuery.inputs.set(sampleQuery);
	if(!tracer.doBsdfPdfQuery(pdfQuery))
	{
		return false;
	}

	math::Spectrum radianceLe;
	emitter->evalEmittedRadiance(Xe, &radianceLe);
	
	if(out_L)          { *out_L = sampleQuery.outputs.L; }
	if(out_pdfW)       { *out_pdfW = pdfQuery.outputs.sampleDirPdfW; }
	if(out_radianceLe) { *out_radianceLe = radianceLe; }
	if(out_Xe)         { *out_Xe = Xe; }

	return true;
}

template<ESidednessPolicy POLICY>
inline bool TDirectLightEstimator<POLICY>::neeSample(
	const SurfaceHit&         X,
	const Time&               time,
	SampleFlow&               sampleFlow,
	math::Vector3R* const     out_L,
	real* const               out_pdfW,
	math::Spectrum* const     out_radianceLe,
	SurfaceHit* const         out_Xe) const
{
	const SurfaceOptics* optics = getSurfaceOptics(X);
	const bool canDoNEE = optics && optics->getAllPhenomena().hasNone(
		{ESurfacePhenomenon::DeltaReflection, ESurfacePhenomenon::DeltaTransmission});
	if(!canDoNEE)
	{
		return false;
	}

	DirectEnergySampleQuery directLightSample;
	directLightSample.in.set(X.getPosition());
	m_scene->genDirectSample(directLightSample, sampleFlow);
	if(!directLightSample.out)
	{
		return false;
	}

	// Before visibility test, make sure the vector to light is long enough to avoid self-intersection
	// (at least 3 deltas, 2 for ray endpoints and 1 for ray body, HACK). Also checks sidedness
	// agreement between real geometry and shading normal.
	const auto toLightVec = directLightSample.out.emitPos - directLightSample.in.targetPos;
	if(toLightVec.lengthSquared() <= math::squared(DL_RAY_DELTA_DIST * 3) ||
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
			DL_RAY_DELTA_DIST, 
			toLightVec.length() * 2,// HACK: to ensure an intersection
			time);

		HitProbe probe;
		if(!m_scene->isIntersecting(visibilityRay, &probe))
		{
			return false;
		}

		SurfaceHit Xe(visibilityRay, probe);
		if(Xe.getDetail().getPrimitive() != directLightSample.out.srcPrimitive)
		{
			return false;
		}

		*out_Xe = Xe;

		if(out_L)          { *out_L = visibilityRay.getDirection(); }
		if(out_pdfW)       { *out_pdfW = directLightSample.out.pdfW; }
		if(out_radianceLe) { *out_radianceLe = directLightSample.out.radianceLe; }
	}
	// Occlusion test
	else
	{
		const Ray visibilityRay(
			X.getPosition(), 
			toLightVec.normalize(), 
			DL_RAY_DELTA_DIST, 
			toLightVec.length() - DL_RAY_DELTA_DIST * 2,
			time);

		if(m_scene->isOccluding(visibilityRay))
		{
			return false;
		}

		if(out_L)          { *out_L = visibilityRay.getDirection(); }
		if(out_pdfW)       { *out_pdfW = directLightSample.out.pdfW; }
		if(out_radianceLe) { *out_radianceLe = directLightSample.out.radianceLe; }
	}

	return true;
}

//template<ESidednessPolicy POLICY>
//inline bool TDirectLightEstimator<POLICY>::misSample(
//	const SurfaceHit&         X,
//	const Time&               time,
//	SampleFlow&               sampleFlow,
//	math::Vector3R* const     out_L,
//	real* const               out_pdfW,
//	math::Spectrum* const     out_radianceLe,
//	SurfaceHit* const         out_Xe) const
//{
//
//}

template<ESidednessPolicy POLICY>
inline real TDirectLightEstimator<POLICY>::neeSamplePdfWUnoccluded(
	const SurfaceHit&     X,
	const SurfaceHit&     Xe,
	const Time&           time) const
{
	// FIXME: redundant pointers
	const Primitive* const emissivePrimitive = Xe.getDetail().getPrimitive();
	const Emitter* const   emitter           = emissivePrimitive->getMetadata()->getSurface().getEmitter();
	PH_ASSERT(emitter);

	return m_scene->calcDirectPdfW(Xe, X.getPosition());
}

template<ESidednessPolicy POLICY>
inline const Primitive& TDirectLightEstimator<POLICY>::getPrimitive(const SurfaceHit& X) const
{
	// Does not make sense to call this method if `X` hits nothing
	PH_ASSERT(X.getDetail().getPrimitive());

	return *(X.getDetail().getPrimitive());
}

template<ESidednessPolicy POLICY>
inline const SurfaceOptics* TDirectLightEstimator<POLICY>::getSurfaceOptics(const SurfaceHit& X) const
{
	const PrimitiveMetadata* meta = getPrimitive(X).getMetadata();
	return meta ? meta->getSurface().getOptics() : nullptr;
}

template<ESidednessPolicy POLICY>
inline const Emitter* TDirectLightEstimator<POLICY>::getEmitter(const SurfaceHit& X) const
{
	const PrimitiveMetadata* meta = getPrimitive(X).getMetadata();
	return meta ? meta->getSurface().getEmitter() : nullptr;
}

}// end namespace ph
