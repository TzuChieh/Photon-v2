#include "Engine/Core/LTA/SurfaceTracer.h"
#include "Engine/Core/LTA/VolumeTracker.h"

namespace ph::lta
{

bool SurfaceTracer::traceNextSurface(
	const Ray&                ray,
	const SidednessAgreement& sidedness,
	const VolumeTracker&      volumeTracker,
	SurfaceHit* const         out_X) const
{
	if(!traceNextSurface(ray, sidedness, out_X))
	{
		return false;
	}

	if(volumeTracker.isTrueHit(*out_X))
	{
		return true;
	}

	// Trace next surface until true hit is found
	while(traceNextSurfaceFrom(*out_X, ray, sidedness, out_X))
	{
		if(volumeTracker.isTrueHit(*out_X))
		{
			return true;
		}
	}

	return false;
}

}// end namespace ph::lta
