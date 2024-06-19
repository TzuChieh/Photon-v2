#pragma once

#include "World/Scene.h"
#include "Core/HitProbe.h"
#include "Core/SurfaceHit.h"
#include "Core/LTA/lta.h"
#include "Core/LTA/SidednessAgreement.h"
#include "Core/LTA/SurfaceHitRefinery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/Color/Spectrum.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Emitter/Emitter.h"

#include <Common/assertion.h>

#include <limits>

namespace ph { class SampleFlow; }

namespace ph::lta
{

/*! @brief Common operations for surface tracing.
This class also handles many subtle cases for surface tracing. You may take the implementations here
as reference if a more fine-grained control is needed for a custom operation.
*/
class SurfaceTracer final
{
public:
	explicit SurfaceTracer(const Scene* scene);

	/*! @brief Find the next surface.
	This variant does not refine the surface hit point. If refining is desired,
	see `traceNextSurfaceFrom()`.
	@param ray The ray that is used for finding the next surface.
	@param sidedness Sidedness policy.
	@return out_X The next surface.
	@return Is the next surface found. Output parameters are not usable if `false` is returned.
	@note If you are tracing from a surface (not a point from the mid-air),
	`traceNextSurfaceFrom(const SurfaceHit&, const Ray&, const SidednessAgreement&, SurfaceHit*)`
	may be more robust.
	*/
	bool traceNextSurface(
		const Ray&                ray, 
		const SidednessAgreement& sidedness, 
		SurfaceHit*               out_X) const;

	/*! @brief Find the next surface from a location.
	This variant also refines the surface hit point before starting the trace.
	@param X The location to start the find from. Can also use the object as `out_X`.
	@param ray The ray that is used for finding the next surface.
	@param sidedness Sidedness policy.
	@return out_X The next surface.
	@return Is the next surface found. Output parameters are not usable if `false` is returned.
	*/
	bool traceNextSurfaceFrom(
		const SurfaceHit&         X,
		const Ray&                ray, 
		const SidednessAgreement& sidedness, 
		SurfaceHit*               out_X) const;

	/*! @brief Uses BSDF sample to trace the next surface.
	@return Is the next surface found. Output parameters are not usable if `false` is returned.
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
	@return Whether the BSDF sample has potential to contribute. Output parameters are not usable if
	`false` is returned.
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

	/*!
	@param out_Le The sampled emitted energy of `Xe` in the opposite direction of incident ray. Does not
	contain any weighting.
	@return Whether the sample has potential to contribute. Output parameters are not usable if `false`
	is returned.
	*/
	bool sampleZeroBounceEmission(
		const SurfaceHit&         Xe, 
		const SidednessAgreement& sidedness,
		math::Spectrum*           out_Le) const;
	
private:
	const Scene& getScene() const;
	
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
	PH_ASSERT(out_X);

	HitProbe probe;
	if(!getScene().isIntersecting(ray, &probe))
	{
		return false;
	}

	*out_X = SurfaceHit(ray, probe, SurfaceHitReason(ESurfaceHitReason::IncidentRay));
	sidedness.adjustForSidednessAgreement(*out_X);

	return sidedness.isSidednessAgreed(*out_X, ray.getDir());
}

inline bool SurfaceTracer::traceNextSurfaceFrom(
	const SurfaceHit&         X,
	const Ray&                ray, 
	const SidednessAgreement& sidedness, 
	SurfaceHit* const         out_X) const
{
	// Not tracing from uninitialized surface hit
	PH_ASSERT(!X.getReason().hasExactly(ESurfaceHitReason::Invalid));

	const Ray refinedRay = SurfaceHitRefinery{X}.escape(ray.getDir());
	return traceNextSurface(refinedRay, sidedness, out_X);
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

	return traceNextSurfaceFrom(
		bsdfSample.inputs.getX(), 
		sampledRay, 
		bsdfSample.context.sidedness, 
		out_X);
}

inline bool SurfaceTracer::doBsdfSample(BsdfSampleQuery& bsdfSample, SampleFlow& sampleFlow) const
{
	const SurfaceHit& X = bsdfSample.inputs.getX();
	const SurfaceOptics* const optics = X.getSurfaceOptics();
	if(!optics)
	{
		return false;
	}

	optics->genBsdfSample(bsdfSample, sampleFlow);

	return bsdfSample.outputs.isMeasurable();
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
		bsdfSample.inputs.getX().getPos(),
		bsdfSample.outputs.getL(),
		0,
		std::numeric_limits<real>::max(),
		bsdfSample.inputs.getX().getTime());

	return true;
}

inline bool SurfaceTracer::doBsdfEvaluation(BsdfEvalQuery& bsdfEval) const
{
	const SurfaceHit& X = bsdfEval.inputs.getX();
	const SurfaceOptics* const optics = X.getSurfaceOptics();
	if(!optics)
	{
		return false;
	}

	optics->calcBsdf(bsdfEval);

	return bsdfEval.outputs.isMeasurable();
}

inline bool SurfaceTracer::doBsdfPdfQuery(BsdfPdfQuery& bsdfPdfQuery) const
{
	const SurfaceHit& X = bsdfPdfQuery.inputs.getX();
	const SurfaceOptics* const optics = X.getSurfaceOptics();
	if(!optics)
	{
		return false;
	}

	optics->calcBsdfPdf(bsdfPdfQuery);

	return bsdfPdfQuery.outputs;
}

inline bool SurfaceTracer::sampleZeroBounceEmission(
	const SurfaceHit&         Xe, 
	const SidednessAgreement& sidedness,
	math::Spectrum* const     out_Le) const
{
	PH_ASSERT(out_Le);

	const auto* const emitter = Xe.getSurfaceEmitter();

	// Sidedness agreement between real geometry and shading normal
	// (do not check for hemisphere--emitter may be back-emitting and this is judged by the emitter)
	if(!emitter ||
	   !sidedness.isSidednessAgreed(Xe, Xe.getIncidentRay().getDir()))
	{
		return false;
	}

	emitter->evalEmittedRadiance(Xe, out_Le);
	return true;
}

inline const Scene& SurfaceTracer::getScene() const
{
	PH_ASSERT(m_scene);

	return *m_scene;
}

}// end namespace ph::lta
