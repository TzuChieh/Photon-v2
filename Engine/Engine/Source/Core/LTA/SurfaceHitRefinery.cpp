#include "Core/LTA/SurfaceHitRefinery.h"
#include "EngineInitSettings.h"

#include <Common/logging.h>

namespace ph::lta
{

ESurfaceRefineMode SurfaceHitRefinery::s_refineMode = ESurfaceRefineMode::Default;
real SurfaceHitRefinery::s_selfIntersectDelta = 0.0002_r;
std::size_t SurfaceHitRefinery::s_numIterations = 2;

#if PH_ENABLE_HIT_EVENT_STATS
SurfaceHitRefinery::HitEventStats SurfaceHitRefinery::s_stats;

void SurfaceHitRefinery::reportStats()
{
	const auto numEvents = s_stats.numEvents.load();
	const auto numFailedEmpiricalEscapes = s_stats.numFailedEmpiricalEscapes.load();
	const auto numFailedIterativeEscapes = s_stats.numFailedIterativeEscapes.load();
	const auto numReintersects = s_stats.numReintersecs.load();

	PH_DEFAULT_LOG(Note,
		"Surface hit refine stats: "
		"{} events, "
		"{} failed initial empirical escapes ({}%), "
		"{} failed iterative escapes ({}%), "
		"{} re-intersects performed ({} per event)",
		numEvents,
		numFailedEmpiricalEscapes,
		numEvents > 0 ? static_cast<double>(numFailedEmpiricalEscapes) / numEvents * 100 : 0.0,
		numFailedIterativeEscapes,
		numEvents > 0 ? static_cast<double>(numFailedIterativeEscapes) / numEvents * 100 : 0.0,
		numReintersects,
		numEvents > 0 ? static_cast<double>(numReintersects) / numEvents : 0.0);
}
#endif

void SurfaceHitRefinery::init(const EngineInitSettings& settings)
{
	s_refineMode = settings.surfaceRefineMode;
	s_selfIntersectDelta = settings.selfIntersectDelta;
	s_numIterations = settings.numIterativeSurfaceRefinements;

#if PH_ENABLE_HIT_EVENT_STATS
	s_stats.numEvents = 0;
	s_stats.numFailedEmpiricalEscapes = 0;
	s_stats.numFailedIterativeEscapes = 0;
	s_stats.numReintersecs = 0;
#endif
}

math::Vector3R SurfaceHitRefinery::iterativeOffsetVec(
	const SurfaceHit& X, 
	const math::Vector3R& dir,
	const std::size_t numIterations)
{
	// Offset in the hemisphere such that we will not bump into ourself (no visible horizon line)
	const auto offsetDir = X.getDetail().getFaceTopology().has(EFaceTopology::Concave)
		? -X.getGeometryNormal() : X.getGeometryNormal();
	const auto escapeDir = offsetDir.dot(dir) > 0.0_r
		? dir.normalize() : -dir.normalize();

	PH_ASSERT_IN_RANGE(offsetDir.lengthSquared(), 0.9_r, 1.1_r);
	PH_ASSERT_IN_RANGE(escapeDir.lengthSquared(), 0.9_r, 1.1_r);

	// First find an offset that results in no intersection
	real maxDist = empiricalOffsetDist(X);
	while(true)
	{
		HitProbe probe;
		Ray ray(X.getPosition() + offsetDir * maxDist, escapeDir, X.getTime());
		if(!reintersect(X, ray, probe))
		{
			break;
		}

		maxDist *= 2.0_r;
	}
	PH_ASSERT_MSG(std::isfinite(maxDist), std::to_string(maxDist));

#if PH_ENABLE_HIT_EVENT_STATS
	if(maxDist > empiricalOffsetDist(X))
	{
		s_stats.markFailedEmpiricalEscape();
	}
#endif

	// Then use bisection method to find the smallest distance that results in no intersection
	real minDist = 0.0_r;
	std::size_t numRefinements = 0;
	while(numRefinements < numIterations)
	{
		const real midDist = (minDist + maxDist) * 0.5_r;
		if(!(minDist < midDist && midDist < maxDist))
		{
			break;
		}

		HitProbe probe;
		Ray ray(X.getPosition() + offsetDir * midDist, escapeDir, X.getTime());
		if(!reintersect(X, ray, probe))
		{
			maxDist = midDist;

			// Only when we found a smaller `maxDist` count as a single refinement
			++numRefinements;
		}
		else
		{
			minDist = midDist;
		}
	}

	const auto offsetVec = SidednessAgreement{ESidednessPolicy::TrustGeometry}.isFrontHemisphere(X, dir)
		? X.getGeometryNormal() * maxDist : X.getGeometryNormal() * -maxDist;

#if PH_ENABLE_HIT_EVENT_STATS
	// Unfortunately we cannot test whether the escape was successful or not for concaves as the
	// ray can have valid self-intersect against them
	if(!X.getDetail().getFaceTopology().has(EFaceTopology::Concave))
	{
		HitProbe probe;
		Ray ray(X.getPosition() + offsetVec, dir.normalize(), X.getTime());
		if(X.reintersect(ray, probe))
		{
			s_stats.markFailedIterativeEscape();
		}
	}
#endif

	return offsetVec;
}

}// end namespace ph::lta
