#pragma once

#include "Core/Estimator/Utility/TSurfaceEventDispatcher.h"
#include "Common/assertion.h"
#include "World/Scene.h"
#include "Core/HitProbe.h"
#include "Core/SurfaceHit.h"
#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"

#include <limits>

namespace ph
{

namespace
{
	const SurfaceOptics* get_surface_optics(const SurfaceHit& X)
	{
		const auto* const metadata = X.getDetail().getPrimitive()->getMetadata();
		PH_ASSERT(metadata);

		const auto* const optics = metadata->getSurface().getOptics();
		PH_ASSERT(optics);

		return optics;
	}
}

template<ESaPolicy POLICY>
inline TSurfaceEventDispatcher<POLICY>::TSurfaceEventDispatcher(
	const Scene* const scene) :
	m_scene(scene)
{
	PH_ASSERT(scene);
}

template<ESaPolicy POLICY>
inline bool TSurfaceEventDispatcher<POLICY>::traceNextSurface(
	const Ray&        ray,
	SurfaceHit* const out_X) const
{
	PH_ASSERT(m_scene && out_X);

	HitProbe probe;
	if(!m_scene->isIntersecting(ray, &probe))
	{
		return false;
	}

	*out_X = SurfaceHit(ray, probe);
	SidednessAgreement(POLICY).adjustForSidednessAgreement(*out_X);

	return SidednessAgreement(POLICY).isSidednessAgreed(*out_X, ray.getDirection());
}

template<ESaPolicy POLICY>
inline bool TSurfaceEventDispatcher<POLICY>::doBsdfSample(
	const SurfaceHit& X,
	BsdfSample&       bsdfSample,
	Ray* const        out_ray) const
{
	PH_ASSERT(m_scene && out_ray);

	if(!X.hasSurfaceOptics() || 
	   !SidednessAgreement(POLICY).isSidednessAgreed(X, bsdfSample.inputs.V))
	{
		return false;
	}

	get_surface_optics(X)->calcBsdfSample(bsdfSample);

	// HACK: hard-coded number
	*out_ray = Ray(X.getPosition(), bsdfSample.outputs.L, 0.0001_r, std::numeric_limits<real>::max());

	return bsdfSample.outputs.isGood() &&
	       SidednessAgreement(POLICY).isSidednessAgreed(X, bsdfSample.outputs.L);
}

template<ESaPolicy POLICY>
inline bool TSurfaceEventDispatcher<POLICY>::doBsdfEvaluation(
	const SurfaceHit& X,
	BsdfEvaluation&   bsdfEvaluation) const
{
	PH_ASSERT(m_scene);

	if(!X.hasSurfaceOptics() ||
	   !SidednessAgreement(POLICY).isSidednessAgreed(X, bsdfEvaluation.inputs.V) ||
	   !SidednessAgreement(POLICY).isSidednessAgreed(X, bsdfEvaluation.inputs.L))
	{
		return false;
	}

	get_surface_optics(X)->calcBsdf(bsdfEvaluation);

	return bsdfEvaluation.outputs.isGood();
}

template<ESaPolicy POLICY>
inline bool TSurfaceEventDispatcher<POLICY>::doBsdfPdfQuery(
	const SurfaceHit& X,
	BsdfPdfQuery&     bsdfPdfQuery) const
{
	PH_ASSERT(m_scene);

	if(!X.hasSurfaceOptics() ||
	   !SidednessAgreement(POLICY).isSidednessAgreed(X, bsdfPdfQuery.inputs.V) ||
	   !SidednessAgreement(POLICY).isSidednessAgreed(X, bsdfPdfQuery.inputs.L))
	{
		return false;
	}

	get_surface_optics(X)->calcBsdfSamplePdfW(bsdfPdfQuery);

	return bsdfPdfQuery.outputs.sampleDirPdfW > 0.0_r;
}

}// end namespace ph