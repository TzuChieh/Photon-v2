#include "Core/LTA/SurfaceHitRefinery.h"
#include "EngineInitSettings.h"

namespace ph::lta
{

real SurfaceHitRefinery::s_selfIntersectDelta = 0.0002_r;

void SurfaceHitRefinery::init(const EngineInitSettings& settings)
{
	s_selfIntersectDelta = settings.selfIntersectDelta;
}

}// end namespace ph::lta
