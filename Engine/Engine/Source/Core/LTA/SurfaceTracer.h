#pragma once

#include "World/Scene.h"
#include "Core/HitProbe.h"
#include "Core/SurfaceHit.h"
#include "Core/LTA/lta.h"
#include "Core/LTA/SidednessAgreement.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"

#include <Common/assertion.h>

#include <limits>

namespace ph { class SampleFlow; }

namespace ph::lta
{

class SurfaceTracer final
{
public:
	explicit SurfaceTracer(const Scene* scene);

	/*!
	@return out_X The next surface.
	@return Is the next surface found.
	*/
	bool traceNextSurface(
		const Ray&                ray, 
		const SidednessAgreement& sidedness, 
		SurfaceHit*               out_X) const;

	/*! @brief Uses BSDF sample to trace the next surface.
	@return Is the next surface found.
	*/
	bool bsdfSampleNextSurface(
		BsdfSampleQuery& bsdfSample,
		SampleFlow&      sampleFlow,
		SurfaceHit*      out_X) const;

	/*!
	@return Whether the BSDF sample has potential to contribute.
	*/
	bool doBsdfSample(BsdfSampleQuery& bsdfSample, SampleFlow& sampleFlow) const;

	/*!
	@return Whether the BSDF sample has potential to contribute.
	*/
	bool doBsdfSample(
		BsdfSampleQuery& bsdfSample, 
		SampleFlow&      sampleFlow, 
		Ray*             out_sampledRay) const;
	
	/*!
	@return Whether the BSDF has potential to contribute.
	*/
	bool doBsdfEvaluation(BsdfEvalQuery& bsdfEval) const;

	/*!
	@return Whether the PDF is non-zero and has a sane value.
	*/
	bool doBsdfPdfQuery(BsdfPdfQuery& bsdfPdfQuery) const;
	
private:
	const Scene& getScene() const;

	static const SurfaceOptics* getSurfaceOptics(const SurfaceHit& X);
	
	const Scene* m_scene;
};

// In-header Implementations:

inline SurfaceTracer::SurfaceTracer(const Scene* const scene)
	: m_scene(scene)
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
	if(!getScene().isIntersecting(ray, &probe))
	{
		return false;
	}

	*out_X = SurfaceHit(ray, probe);
	sidedness.adjustForSidednessAgreement(*out_X);

	return sidedness.isSidednessAgreed(*out_X, ray.getDirection());
}

inline bool SurfaceTracer::bsdfSampleNextSurface(
	BsdfSampleQuery&  bsdfSample,
	SampleFlow&       sampleFlow,
	SurfaceHit* const out_X) const
{
	Ray sampledRay;
	if(!doBsdfSample(bsdfSample, sampleFlow, &sampledRay))
	{
		return false;
	}

	return traceNextSurface(sampledRay, bsdfSample.context.sidedness, out_X);
}

inline bool SurfaceTracer::doBsdfSample(BsdfSampleQuery& bsdfSample, SampleFlow& sampleFlow) const
{
	PH_ASSERT(m_scene);

	const SurfaceHit&         X         = bsdfSample.inputs.getX();
	const SidednessAgreement& sidedness = bsdfSample.context.sidedness;

	if(!X.hasSurfaceOptics() || 
	   !sidedness.isSidednessAgreed(X, bsdfSample.inputs.getV()))
	{
		return false;
	}

	getSurfaceOptics(X)->calcBsdfSample(bsdfSample, sampleFlow);

	return bsdfSample.outputs.isMeasurable() &&
	       sidedness.isSidednessAgreed(X, bsdfSample.outputs.getL());
}

inline bool SurfaceTracer::doBsdfSample(
	BsdfSampleQuery& bsdfSample,
	SampleFlow&      sampleFlow,
	Ray* const       out_sampledRay) const
{
	if(!doBsdfSample(bsdfSample, sampleFlow))
	{
		return false;
	}

	PH_ASSERT(out_sampledRay);
	*out_sampledRay = Ray(
		bsdfSample.inputs.getX().getPosition(),
		bsdfSample.outputs.getL(),
		lta::self_intersect_delta,
		std::numeric_limits<real>::max(),
		bsdfSample.inputs.getX().getIncidentRay().getTime());

	return true;
}

inline bool SurfaceTracer::doBsdfEvaluation(BsdfEvalQuery& bsdfEval) const
{
	PH_ASSERT(m_scene);

	const SurfaceHit&         X         = bsdfEval.inputs.getX();
	const SidednessAgreement& sidedness = bsdfEval.context.sidedness;

	if(!X.hasSurfaceOptics() ||
	   !sidedness.isSidednessAgreed(X, bsdfEval.inputs.getV()) ||
	   !sidedness.isSidednessAgreed(X, bsdfEval.inputs.getL()))
	{
		return false;
	}

	getSurfaceOptics(X)->calcBsdf(bsdfEval);

	return bsdfEval.outputs.isMeasurable();
}

inline bool SurfaceTracer::doBsdfPdfQuery(BsdfPdfQuery& bsdfPdfQuery) const
{
	PH_ASSERT(m_scene);

	const SurfaceHit&         X         = bsdfPdfQuery.inputs.getX();
	const SidednessAgreement& sidedness = bsdfPdfQuery.context.sidedness;

	if(!X.hasSurfaceOptics() ||
	   !sidedness.isSidednessAgreed(X, bsdfPdfQuery.inputs.getV()) ||
	   !sidedness.isSidednessAgreed(X, bsdfPdfQuery.inputs.getL()))
	{
		return false;
	}

	getSurfaceOptics(X)->calcBsdfSamplePdfW(bsdfPdfQuery);

	return bsdfPdfQuery.outputs.getSampleDirPdfW() > 0.0_r;
}

inline const Scene& SurfaceTracer::getScene() const
{
	PH_ASSERT(m_scene);

	return *m_scene;
}

inline const SurfaceOptics* SurfaceTracer::getSurfaceOptics(const SurfaceHit& X)
{
	// Does not make sense to call this method if `X` hits nothing
	PH_ASSERT(X.getDetail().getPrimitive());

	const auto* const meta = X.getDetail().getPrimitive()->getMetadata();
	return meta ? meta->getSurface().getOptics() : nullptr;
}

}// end namespace ph::lta
