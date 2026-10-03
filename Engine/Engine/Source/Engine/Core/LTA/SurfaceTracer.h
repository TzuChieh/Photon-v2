#pragma once

#include "Engine/World/Scene.h"
#include "Engine/Core/HitProbe.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/LTA/lta.h"
#include "Engine/Core/LTA/SidednessAgreement.h"
#include "Engine/Core/LTA/SurfaceHitRefinery.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Engine/Core/Ray.h"
#include "Engine/Math/math.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/Emitter/SurfaceEmitter.h"

#include <Common/assertion.h>

#include <limits>
#include <type_traits>
#include <utility>

namespace ph { class SampleFlow; }

namespace ph::lta
{

class VolumeTracker;

/*! @brief Common operations for surface tracing.
This class also handles many subtle cases for surface tracing. You may take the implementations here
as reference if a more fine-grained control is needed for a custom operation.
*/
class SurfaceTracer final
{
public:
	explicit SurfaceTracer(const Scene* scene);

	/*! @brief Find the next physical surface.
	This variant does not refine the surface hit point. If refining is desired,
	see `traceNextSurfaceFrom()`.
	@param ray The ray that is used for finding the next surface.
	@param out_X The next physical surface. Usable only when `true` is returned.
	@param out_boundedRay Optional input ray bounded by the physical hit, or the full ray on a miss.
	Always usable when provided.
	@return Whether a physical surface is hit, regardless of sidedness.
	@note If you are tracing from a surface (not a point from the mid-air),
	`traceNextSurfaceFrom()` may be more robust.
	*/
	bool traceNextSurface(const Ray& ray, SurfaceHit* out_X, Ray* out_boundedRay = nullptr) const;

	/*! @brief Find the next surface from a location.
	This variant also refines the surface hit point before starting the trace.
	@param X The location to start the find from. Can also use the same object as `out_X`.
	@param ray The ray that is used for finding the next surface. Must be originated from `X`.
	@param out_X The next physical surface, usable only when `true` is returned.
	@param out_boundedRay Optional refined ray ending at the hit, or the full refined ray on a miss.
	Always usable when provided.
	@return Whether a physical surface is hit, regardless of sidedness.
	*/
	bool traceNextSurfaceFrom(
		const SurfaceHit& X,
		const Ray&        ray,
		SurfaceHit*       out_X,
		Ray*              out_boundedRay = nullptr) const;

	/*! @brief Find the next physical surface while tracking volumes.
	@param out_X The next physical surface. Usable only when `true` is returned.
	@param out_boundedRay Optional input ray bounded by the physical hit, or the full ray on a miss.
	Always usable when provided.
	*/
	bool traceNextSurface(
		const Ray&     ray,
		VolumeTracker& volumeTracker,
		SurfaceHit*    out_X,
		Ray*           out_boundedRay = nullptr) const;

	/*!
	@param out_boundedRay Optional refined ray ending at the hit, or the full refined ray on a miss.
	Always usable when provided.
	*/
	bool traceNextSurfaceFrom(
		const SurfaceHit& X,
		const Ray&        ray,
		VolumeTracker&    volumeTracker,
		SurfaceHit*       out_X,
		Ray*              out_boundedRay = nullptr) const;

	/*! @brief Refine a ray originating at `X` to avoid self-intersection.
	*/
	Ray getRefinedRayOriginatedFrom(const SurfaceHit& X, const Ray& ray) const;

	/*! @brief Uses BSDF sample to trace the next surface.
	@return Whether the next surface agrees with `bsdfSample.context.sidedness`.
	Output parameters are not usable if `false` is returned.
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

	/*! @brief Visit nonblocking emitter hits within a ray segment, regardless of sidedness.
	@tparam FEATURE Allowed features. Must enable at least one.
	@param ray Bounds the segment to find non-blocking emitters.
	@param visitor Called as `visitor(SurfaceHit& emitterHit)`. The hit is local to the callback.
	*/
	template<EEmitterFeatureSet FEATURE = EEmitterFeatureSet::Default, typename Visitor>
	void forEachNonBlockingEmitterHit(
		const Ray& ray,
		Visitor&&  visitor) const;
	
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
	const Ray&        ray,
	SurfaceHit* const out_X,
	Ray* const        out_boundedRay) const
{
	PH_ASSERT(out_X);

	HitProbe probe;
	if(!getScene().isIntersecting(ray, &probe))
	{
		if(out_boundedRay) { *out_boundedRay = ray; }
		return false;
	}

	*out_X = SurfaceHit(ray, probe, ESurfaceHitReason::IncidentRay);
	if(out_boundedRay)
	{
		*out_boundedRay = Ray(
			ray.getOrigin(),
			ray.getDir(),
			ray.getMinT(),
			out_X->getDetail().getRayT(),
			ray.getTime());
	}
	return true;
}

template<EEmitterFeatureSet FEATURE, typename Visitor>
inline void SurfaceTracer::forEachNonBlockingEmitterHit(
	const Ray& ray,
	Visitor&&  visitor) const
{
	static_assert(std::is_invocable_r_v<void, Visitor&, SurfaceHit&>);

	getScene().forEachNonBlockingEmitterHit<FEATURE>(
		ray,
		std::forward<Visitor>(visitor));
}

inline bool SurfaceTracer::traceNextSurfaceFrom(
	const SurfaceHit& X,
	const Ray&        ray,
	SurfaceHit* const out_X,
	Ray* const        out_boundedRay) const
{
	// Not tracing from uninitialized surface hit
	PH_ASSERT(!X.getReason().hasExactly(ESurfaceHitReason::Invalid));

	const Ray refinedRay = getRefinedRayOriginatedFrom(X, ray);
	return traceNextSurface(refinedRay, out_X, out_boundedRay);
}

inline bool SurfaceTracer::traceNextSurfaceFrom(
	const SurfaceHit& X,
	const Ray&        ray,
	VolumeTracker&    volumeTracker,
	SurfaceHit* const out_X,
	Ray* const        out_boundedRay) const
{
	// Not tracing from uninitialized surface hit
	PH_ASSERT(!X.getReason().hasExactly(ESurfaceHitReason::Invalid));

	const Ray refinedRay = getRefinedRayOriginatedFrom(X, ray);
	return traceNextSurface(refinedRay, volumeTracker, out_X, out_boundedRay);
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

	if(!traceNextSurfaceFrom(bsdfSample.inputs.getX(), sampledRay, out_X))
	{
		return false;
	}

	const SidednessAgreement& sidedness = bsdfSample.context.sidedness;
	sidedness.adjustForSidednessAgreement(*out_X);
	return sidedness.isSidednessAgreed(*out_X, out_X->getIncidentRay().getDir());
}

inline bool SurfaceTracer::doBsdfSample(BsdfSampleQuery& bsdfSample, SampleFlow& sampleFlow) const
{
	const SurfaceHit& X = bsdfSample.inputs.getX();
	const SurfaceOptics& optics = X.getSurfaceOptics();

	optics.genBsdfSample(bsdfSample, sampleFlow);

	return bsdfSample.outputs.isContributable();
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
	const SurfaceOptics& optics = X.getSurfaceOptics();

	optics.calcBsdf(bsdfEval);

	return bsdfEval.outputs.isContributable();
}

inline bool SurfaceTracer::doBsdfPdfQuery(BsdfPdfQuery& bsdfPdfQuery) const
{
	const SurfaceHit& X = bsdfPdfQuery.inputs.getX();
	const SurfaceOptics& optics = X.getSurfaceOptics();

	optics.calcBsdfPdf(bsdfPdfQuery);

	return bsdfPdfQuery.outputs;
}

inline bool SurfaceTracer::sampleZeroBounceEmission(
	const SurfaceHit&         Xe, 
	const SidednessAgreement& sidedness,
	math::Spectrum* const     out_Le) const
{
	PH_ASSERT(out_Le);

	const SurfaceEmitter& emitter = Xe.getSurfaceEmitter();

	// Sidedness agreement between real geometry and shading normal
	// (do not check for hemisphere--emitter may be back-emitting and this is judged by the emitter itself)
	if(!Xe.getMetadata().getSurface().isEmissive() ||
	   emitter.getFeatureSet().hasNo(EEmitterFeatureSet::ZeroBounceSample) ||
	   !sidedness.isSidednessAgreed(Xe, Xe.getIncidentRay().getDir()))
	{
		return false;
	}

	emitter.evalEmittedEnergy(Xe, out_Le);
	return true;
}

inline const Scene& SurfaceTracer::getScene() const
{
	PH_ASSERT(m_scene);

	return *m_scene;
}

inline Ray SurfaceTracer::getRefinedRayOriginatedFrom(const SurfaceHit& X, const Ray& ray) const
{
	constexpr real ALLOWED_ERROR = 1e-4_r;

	// `ray` must be originated from `X`; may be slightly different due to optimiation or numerical error
	// and anything larger than `ALLOWED_ERROR` will be considered a bug
	PH_ASSERT_MSG(ray.getOrigin().isNear(X.getPos(), ALLOWED_ERROR),
		"ray: " + ray.getOrigin().toString() + ", X: " + X.getPos().toString());

	Ray refinedRay = SurfaceHitRefinery{X}.escape(ray.getDir());

	// Limit the max T, as escaped ray has longest length by default
	refinedRay.setMaxT(math::clamp(refinedRay.getMaxT(), ray.getMinT(), ray.getMaxT()));

	return refinedRay;
}

}// end namespace ph::lta
