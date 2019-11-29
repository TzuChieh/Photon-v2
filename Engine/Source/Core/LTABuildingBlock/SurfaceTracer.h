#pragma once

#include "Utility/INoncopyable.h"
#include "Common/assertion.h"
#include "World/Scene.h"
#include "Core/HitProbe.h"
#include "Core/SurfaceHit.h"
#include "Core/LTABuildingBlock/SidednessAgreement.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
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

class SampleFlow;

class SurfaceTracer final : public INoncopyable
{
public:
	explicit SurfaceTracer(const Scene* scene);

	bool traceNextSurface(
		const Ray&                ray, 
		const SidednessAgreement& sidedness, 
		SurfaceHit*               out_X) const;

	bool doBsdfSample(
		BsdfSampleQuery& bsdfSample,
		SampleFlow&      sampleFlow,
		Ray*             out_ray) const;

	bool doBsdfEvaluation(BsdfEvalQuery& bsdfEval) const;
	bool doBsdfPdfQuery(BsdfPdfQuery& bsdfPdfQuery) const;
	
private:
	const Scene* m_scene;

	static const SurfaceOptics* getSurfaceOptics(const SurfaceHit& X);
};

// In-header Implementations:

inline const SurfaceOptics* SurfaceTracer::getSurfaceOptics(const SurfaceHit& X)
{
	const auto* const metadata = X.getDetail().getPrimitive()->getMetadata();
	PH_ASSERT(metadata);

	const auto* const optics = metadata->getSurface().getOptics();
	PH_ASSERT(optics);

	return optics;
}

inline SurfaceTracer::SurfaceTracer(const Scene* const scene) :
	m_scene(scene)
{
	PH_ASSERT(scene);
}

inline bool SurfaceTracer::traceNextSurface(
	const Ray&                ray,
	const SidednessAgreement& sidedness,
	SurfaceHit* const         out_X) const
{
	PH_ASSERT(m_scene);
	PH_ASSERT(out_X);

	HitProbe probe;
	if(!m_scene->isIntersecting(ray, &probe))
	{
		return false;
	}

	*out_X = SurfaceHit(ray, probe);
	sidedness.adjustForSidednessAgreement(*out_X);

	return sidedness.isSidednessAgreed(*out_X, ray.getDirection());
}

inline bool SurfaceTracer::doBsdfSample(
	BsdfSampleQuery& bsdfSample, 
	SampleFlow&      sampleFlow, 
	Ray* const       out_ray) const
{
	PH_ASSERT(m_scene);
	PH_ASSERT(out_ray);

	const SurfaceHit&         X         = bsdfSample.inputs.X;
	const SidednessAgreement& sidedness = bsdfSample.context.sidedness;

	if(!X.hasSurfaceOptics() || 
	   !sidedness.isSidednessAgreed(X, bsdfSample.inputs.V))
	{
		return false;
	}

	getSurfaceOptics(X)->calcBsdfSample(bsdfSample, sampleFlow);

	// HACK: hard-coded number
	*out_ray = Ray(X.getPosition(), bsdfSample.outputs.L, 0.0001_r, std::numeric_limits<real>::max());

	return bsdfSample.outputs.isMeasurable() &&
	       sidedness.isSidednessAgreed(X, bsdfSample.outputs.L);
}

inline bool SurfaceTracer::doBsdfEvaluation(BsdfEvalQuery& bsdfEval) const
{
	PH_ASSERT(m_scene);

	const SurfaceHit&         X         = bsdfEval.inputs.X;
	const SidednessAgreement& sidedness = bsdfEval.context.sidedness;

	if(!X.hasSurfaceOptics() ||
	   !sidedness.isSidednessAgreed(X, bsdfEval.inputs.V) ||
	   !sidedness.isSidednessAgreed(X, bsdfEval.inputs.L))
	{
		return false;
	}

	getSurfaceOptics(X)->calcBsdf(bsdfEval);

	return bsdfEval.outputs.isGood();
}

inline bool SurfaceTracer::doBsdfPdfQuery(BsdfPdfQuery& bsdfPdfQuery) const
{
	PH_ASSERT(m_scene);

	const SurfaceHit&         X         = bsdfPdfQuery.inputs.X;
	const SidednessAgreement& sidedness = bsdfPdfQuery.context.sidedness;

	if(!X.hasSurfaceOptics() ||
	   !sidedness.isSidednessAgreed(X, bsdfPdfQuery.inputs.V) ||
	   !sidedness.isSidednessAgreed(X, bsdfPdfQuery.inputs.L))
	{
		return false;
	}

	getSurfaceOptics(X)->calcBsdfSamplePdfW(bsdfPdfQuery);

	return bsdfPdfQuery.outputs.sampleDirPdfW > 0.0_r;
}

}// end namespace ph
