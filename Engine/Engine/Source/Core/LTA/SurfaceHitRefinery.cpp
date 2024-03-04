#include "Core/LTA/SurfaceHitRefinery.h"
#include "EngineInitSettings.h"

#include <Common/logging.h>

namespace ph::lta
{

ESurfaceRefineMode SurfaceHitRefinery::s_refineMode = ESurfaceRefineMode::Default;
real SurfaceHitRefinery::s_selfIntersectDelta = 0.0002_r;

#if PH_ENABLE_HIT_EVENT_STATS
SurfaceHitRefinery::HitEventStats SurfaceHitRefinery::s_stats;

void SurfaceHitRefinery::reportStats()
{
	const auto numEvents = s_stats.numEvents.load();
	const auto numFailedEmpiricalEscapes = s_stats.numFailedEmpiricalEscapes.load();
	const auto numIterations = s_stats.numIterations.load();

	PH_DEFAULT_LOG(Note,
		"Surface hit refine stats: "
		"{} events, "
		"{} failed initial empirical escapes ({}%), "
		"{} iterations performed ({} per event)",
		numEvents,
		numFailedEmpiricalEscapes,
		numEvents > 0 ? static_cast<double>(numFailedEmpiricalEscapes) / numEvents * 100 : 0.0,
		numIterations,
		numEvents > 0 ? static_cast<double>(numIterations) / numEvents : 0.0);
}
#endif

void SurfaceHitRefinery::init(const EngineInitSettings& settings)
{
	s_refineMode = settings.surfaceRefineMode;
	s_selfIntersectDelta = settings.selfIntersectDelta;

#if PH_ENABLE_HIT_EVENT_STATS
	s_stats.numEvents = 0;
	s_stats.numFailedEmpiricalEscapes = 0;
	s_stats.numIterations = 0;
#endif
}

math::Vector3R SurfaceHitRefinery::iterativeOffsetVec(
	const SurfaceHit& X, 
	const math::Vector3R& dir,
	const std::size_t numIterations)
{
	const auto N = X.getGeometryNormal();
	const auto unitDir = dir.normalize();

	PH_ASSERT_IN_RANGE(N.lengthSquared(), 0.9_r, 1.1_r);
	PH_ASSERT_IN_RANGE(unitDir.lengthSquared(), 0.9_r, 1.1_r);

	// Offset in the hemisphere of N, so we will not bump into ourself
	// (TODO: for some shapes we need more logics, e.g., offset into -N for concaves)
	const auto offsetDir = SidednessAgreement{ESidednessPolicy::TrustGeometry}.isFrontHemisphere(X, dir)
		? N : -N;

	// First find an offset that results in no intersection
	real maxDist = empiricalOffsetDist(X);
	while(true)
	{
		HitProbe probe;
		Ray ray(X.getPosition() + offsetDir * maxDist, unitDir, X.getTime());
		if(!X.reintersect(ray, probe))
		{
			break;
		}

		maxDist *= 2.0_r;

#if PH_ENABLE_HIT_EVENT_STATS
		s_stats.markFailedEmpiricalEscape();
#endif
	}
	PH_ASSERT_MSG(std::isfinite(maxDist), std::to_string(maxDist));

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
		Ray ray(X.getPosition() + offsetDir * midDist, unitDir, X.getTime());
		if(!X.reintersect(ray, probe))
		{
			maxDist = midDist;
		}
		else
		{
			minDist = midDist;
		}

		++numRefinements;

#if PH_ENABLE_HIT_EVENT_STATS
		s_stats.markIteration();
#endif
	}

	return offsetDir * maxDist;
}

}// end namespace ph::lta
