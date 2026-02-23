#include "Engine/Core/LTA/SurfaceTracer.h"
#include "Engine/Core/LTA/VolumeTracker.h"

namespace ph::lta
{

namespace
{

inline Ray trim_ray_tail(const Ray& ray, const math::Vector3R& whereToTrim, const real lengthToTrim)
{
	// Assuming `ray` pass through `whereToTrim`
	return Ray(
		whereToTrim,
		ray.getDir(),
		0,
		ray.getSegment().getDeltaT() - lengthToTrim,
		ray.getTime());
}

}// end anonymous namespace

bool SurfaceTracer::traceNextSurface(
	const Ray&                ray,
	const SidednessAgreement& sidedness,
	VolumeTracker&            volumeTracker,
	SurfaceHit* const         out_X) const
{
	PH_ASSERT(out_X);
	SurfaceHit& X = *out_X;

	HitProbe probe;
	if(!getScene().isIntersecting(ray, &probe))
	{
		return false;
	}

	X = SurfaceHit(ray, probe, SurfaceHitReasons(ESurfaceHitReason::IncidentRay));

	// For false hits, do not use strict policy as our intension is to cull geometry. Strict policy can result
	// in premature hit termination while the geometry can be skipped.
	const SidednessAgreement falseHitSidedness{ESidednessPolicy::TrustGeometry};

	// Trace next surface until true hit is found
	Ray remainingRay = ray;
	while(!volumeTracker.isTrueHit(X))
	{
		const real advancedT = X.getDetail().getRayT() - X.getRay().getMinT();
		if(advancedT <= 0)
		{
			return false;
		}

		// False hit implies passing through a surface
		if(falseHitSidedness.isBackHemisphere(X, remainingRay.getDir()))
		{
			volumeTracker.enterSurface(X);
		}
		else
		{
			volumeTracker.exitSurface(X);
		}

		remainingRay = trim_ray_tail(X.getRay(), X.getPos(), advancedT);
		remainingRay = getRefinedRayOriginatedFrom(X, remainingRay);
		if(!getScene().isIntersecting(remainingRay, &probe))
		{
			return false;
		}
		else
		{
			X = SurfaceHit(remainingRay, probe, SurfaceHitReasons(ESurfaceHitReason::IncidentRay));
		}
	}

	sidedness.adjustForSidednessAgreement(X);
	return sidedness.isSidednessAgreed(X, remainingRay.getDir());
}

}// end namespace ph::lta
