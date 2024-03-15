#include "Core/LTA/SurfaceHitRefinery.h"
#include "EngineInitSettings.h"

#include <Common/logging.h>

namespace ph::lta
{

ESurfaceRefineMode SurfaceHitRefinery::s_refineMode;
real SurfaceHitRefinery::s_selfIntersectDelta;
std::size_t SurfaceHitRefinery::s_numIterations;

#if PH_ENABLE_HIT_EVENT_STATS
SurfaceHitRefinery::HitEventStats SurfaceHitRefinery::s_stats;

void SurfaceHitRefinery::reportStats()
{
	const auto numEvents = s_stats.numEvents.load();
	const auto numFailedEmpiricalEscapes = s_stats.numFailedEmpiricalEscapes.load();
	const auto numFailedInitialEscapes = s_stats.numFailedInitialEscapes.load();
	const auto numFailedIterativeEscapes = s_stats.numFailedIterativeEscapes.load();
	const auto numReintersects = s_stats.numReintersecs.load();

	PH_DEFAULT_LOG(Note,
		"Surface hit refine stats: "
		"{} events, "
		"{} failed empirical escapes ({}%), "
		"{} failed initial escapes ({}%), "
		"{} failed iterative escapes ({}%), "
		"{} re-intersects performed ({} per event)",
		numEvents,
		numFailedEmpiricalEscapes,
		numEvents > 0 ? static_cast<double>(numFailedEmpiricalEscapes) / numEvents * 100 : 0.0,
		numFailedInitialEscapes,
		numEvents > 0 ? static_cast<double>(numFailedInitialEscapes) / numEvents * 100 : 0.0,
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
	s_stats.numFailedInitialEscapes = 0;
	s_stats.numFailedIterativeEscapes = 0;
	s_stats.numReintersecs = 0;
#endif
}

auto SurfaceHitRefinery::iterativeOffset(
	const SurfaceHit& X, 
	const math::Vector3R& dir,
	const std::size_t numIterations)
-> IterativeOffsetResult
{
	// Possibly fallback to empirical offset
	if(!canVerifyOffset(X, dir))
	{
		return {
			.offset = empiricalOffsetVec(X, dir),
			.unitDir = dir.normalize(),
			.maxDistance = std::numeric_limits<real>::max()};
	}

	// Offset in the hemisphere such that we will not bump into ourself
	const auto N = X.getGeometryNormal();
	const auto offsetDir = N.dot(dir) > 0.0_r ? N : -N;
	const auto escapeDir = dir.normalize();
	const auto escapeCos = math::clamp(escapeDir.dot(offsetDir), 0.0_r, 1.0_r);

	PH_ASSERT_IN_RANGE(offsetDir.lengthSquared(), 0.9_r, 1.1_r);
	PH_ASSERT_IN_RANGE(escapeDir.lengthSquared(), 0.9_r, 1.1_r);

	// Use mean error for more accuracy (~50% chance with failed initial escape)
	const auto initialOffsetDist = meanErrorOffsetDist(X) + std::numeric_limits<real>::min();

	// First find an offset that results in no intersection
	real maxDist = initialOffsetDist;
	real minDist = 0.0_r;
	while(true)
	{
		HitProbe probe;
		Ray ray(X.getPosition() + offsetDir * maxDist, escapeDir, X.getTime());
		if(!reintersect(X, ray, probe))
		{
			break;
		}

		maxDist += probe.getHitRayT() * escapeCos;
		minDist = maxDist;
		maxDist *= 2.0_r;
	}
	PH_ASSERT_MSG(std::isfinite(maxDist), std::to_string(maxDist));

#if PH_ENABLE_HIT_EVENT_STATS
	if(maxDist > initialOffsetDist)
	{
		s_stats.markFailedInitialEscape();
	}
#endif

	// Then use bisection method to find the smallest distance that results in no intersection
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
			minDist = math::clamp(midDist + probe.getHitRayT() * escapeCos, midDist, maxDist);
		}
	}

	const auto offsetVec = offsetDir * maxDist;

#if PH_ENABLE_HIT_EVENT_STATS
	if(!verifyOffset(X, dir, X.getPosition(), offsetVec))
	{
		s_stats.markFailedIterativeEscape();
	}
#endif

	return {
		.offset = offsetVec,
		.unitDir = escapeDir,
		.maxDistance = std::numeric_limits<real>::max()};
}

auto SurfaceHitRefinery::iterativeMutualOffset(
	const SurfaceHit& X,
	const SurfaceHit& X2,
	const math::Vector3R& dir,
	const std::size_t numIterations)
-> IterativeOffsetResult
{
	const IterativeOffsetResult resultX = iterativeOffset(X, dir, numIterations);
	PH_ASSERT_GT(resultX.maxDistance, 0.0_r);

	// Now we have escaped from `X`. To escape `X2` from `X`, we use the method from `iterativeOffset()`,
	// except that we are now iteratively reducing the distance from `X2` until there is no intersection.
	// This way, we can keep the direction from `X` unchanged. The process assumes reducing the distance
	// will not defeat the efforts done on `X`.

	// Use max error under the assumption that `X2` is typically further and the accuracy on
	// that end matters less (~0% chance with failed initial escape)
	const auto initialOffsetDist = maxErrorOffsetDist(X2) + std::numeric_limits<real>::min();
	const auto resultOrigin = X.getPosition() + resultX.offset;
	const auto distanceToX2 = (resultOrigin - X2.getPosition()).length();
	const auto escapeDir2 = -resultX.unitDir;

	// Possibly fallback to empirical offset
	if(!canVerifyOffset(X2, escapeDir2))
	{
		return {
			.offset = resultX.offset,
			.unitDir = resultX.unitDir,
			.maxDistance = std::max(distanceToX2 - initialOffsetDist, 0.0_r)};
	}

	// First find an offset that results in no intersection
	real maxDist = initialOffsetDist;
	real minDist = 0.0_r;
	while(distanceToX2 > maxDist)
	{
		HitProbe probe;
		Ray ray(
			resultOrigin,
			resultX.unitDir, 
			0, 
			distanceToX2 - maxDist,
			X.getTime());
		if(!reintersect(X2, ray, probe))
		{
			break;
		}

		maxDist = std::max(distanceToX2 - probe.getHitRayT(), maxDist);
		minDist = maxDist;
		maxDist *= 2.0_r;
	}
	PH_ASSERT_MSG(std::isfinite(maxDist), std::to_string(maxDist));

#if PH_ENABLE_HIT_EVENT_STATS
	if(maxDist > initialOffsetDist)
	{
		s_stats.markFailedInitialEscape();
	}
#endif

	// Then use bisection method to find the smallest distance that results in no intersection
	std::size_t numRefinements = 0;
	while(distanceToX2 > maxDist && numRefinements < numIterations)
	{
		const real midDist = (minDist + maxDist) * 0.5_r;
		if(!(minDist < midDist && midDist < maxDist))
		{
			break;
		}

		HitProbe probe;
		Ray ray(
			resultOrigin,
			resultX.unitDir,
			0,
			distanceToX2 - midDist,
			X.getTime());
		if(!reintersect(X2, ray, probe))
		{
			maxDist = midDist;

			// Only when we found a smaller `maxDist` count as a single refinement
			++numRefinements;
		}
		else
		{
			minDist = math::clamp(distanceToX2 - probe.getHitRayT(), midDist, maxDist);
		}
	}

	const auto rayLength = std::max(distanceToX2 - maxDist, 0.0_r);

#if PH_ENABLE_HIT_EVENT_STATS
	if(!verifyOffset(X, dir, X.getPosition(), resultX.offset, rayLength))
	{
		s_stats.markFailedIterativeEscape();
	}

	if(!verifyOffset(X2, dir, X.getPosition(), resultX.offset, rayLength))
	{
		s_stats.markFailedIterativeEscape();
	}
#endif

	return {
		.offset = resultX.offset,
		.unitDir = resultX.unitDir,
		.maxDistance = rayLength};
}

}// end namespace ph::lta
