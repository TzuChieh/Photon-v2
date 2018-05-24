#pragma once

#include "Core/Integrator/Utility/TSurfaceEventDispatcher.h"
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

template<ESidednessAgreement SA>
inline TSurfaceEventDispatcher<SA>::TSurfaceEventDispatcher(
	const Scene* const scene) :
	m_scene(scene)
{
	PH_ASSERT(scene);
}

template<ESidednessAgreement SA>
inline bool TSurfaceEventDispatcher<SA>::traceNextSurface(
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
	adjustForSidednessAgreement(*out_X);

	return isSidednessAgreed(*out_X, ray.getDirection());
}

template<ESidednessAgreement SA>
inline bool TSurfaceEventDispatcher<SA>::doBsdfSample(
	const SurfaceHit& X,
	BsdfSample&       bsdfSample,
	Ray* const        out_ray) const
{
	PH_ASSERT(m_scene && out_ray);

	if(!X.hasSurfaceOptics() || 
	   !isSidednessAgreed(X, bsdfSample.inputs.V))
	{
		return false;
	}

	get_surface_optics(X)->genBsdfSample(bsdfSample);

	return bsdfSample.outputs.isGood() &&
	       isSidednessAgreed(X, bsdfSample.outputs.L);
}

template<ESidednessAgreement SA>
inline bool TSurfaceEventDispatcher<SA>::doBsdfEvaluation(
	const SurfaceHit& X,
	BsdfEvaluation&   bsdfEvaluation) const
{
	PH_ASSERT(m_scene);

	if(!X.hasSurfaceOptics() ||
	   !isSidednessAgreed(X, bsdfEvaluation.inputs.V) ||
	   !isSidednessAgreed(X, bsdfEvaluation.inputs.L))
	{
		return false;
	}

	get_surface_optics(X)->evalBsdf(bsdfEvaluation);

	return bsdfEvaluation.outputs.isGood();
}

template<ESidednessAgreement SA>
inline bool TSurfaceEventDispatcher<SA>::doBsdfPdfQuery(
	const SurfaceHit& X,
	BsdfPdfQuery&     bsdfPdfQuery) const
{
	PH_ASSERT(m_scene);

	if(!X.hasSurfaceOptics() ||
	   !isSidednessAgreed(X, bsdfPdfQuery.inputs.V) ||
	   !isSidednessAgreed(X, bsdfPdfQuery.inputs.L))
	{
		return false;
	}

	get_surface_optics(X)->calcBsdfSamplePdf(bsdfPdfQuery);

	return bsdfPdfQuery.outputs.sampleDirPdfW > 0.0_r;
}

template<ESidednessAgreement SA>
inline bool TSurfaceEventDispatcher<SA>::isSidednessAgreed(
	const SurfaceHit& X,
	const Vector3R&   targetVector) const
{
	if constexpr(SA == ESidednessAgreement::DO_NOT_CARE)
	{
		return true;
	}
	else
	{
		const Vector3R& Ng = X.getGeometryNormal();
		const Vector3R& Ns = X.getShadingNormal();

		return Ng.dot(targetVector) * Ns.dot(targetVector) > 0.0_r;
	}
}

template<ESidednessAgreement SA>
inline void TSurfaceEventDispatcher<SA>::adjustForSidednessAgreement(
	SurfaceHit& X) const
{
	// currently no adjustment
}

}// end namespace ph