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

	if(!traceNextSurface(ray, sidedness, &X))
	{
		return false;
	}

	// Trace next surface until true hit is found
	while(!volumeTracker.isTrueHit(X))
	{
		// False hit implies passing through a surface
		// FIXME: can also enter
		volumeTracker.exitSurface(X);

		const Ray remainingRay = trim_ray_tail(X.getRay(), X.getPos(), X.getDetail().getRayT() - X.getRay().getMinT());
		if(!traceNextSurfaceFrom(X, remainingRay, sidedness, &X))
		{
			return false;
		}
	}

	return true;
}

}// end namespace ph::lta
