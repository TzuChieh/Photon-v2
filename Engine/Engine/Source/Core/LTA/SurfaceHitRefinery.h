#pragma once

#include "Core/SurfaceHit.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/math.h"
#include "ESurfaceRefineMode.h"

#include <Common/config.h>
#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <cmath>
#include <limits>
#include <optional>
#include <atomic>
#include <utility>

namespace ph { class EngineInitSettings; }

namespace ph::lta
{

/*! @brief Algorithms for various hit point adjustments.
For surface escaping routines, the generated ray is not guaranteed to "actually" avoid the surface.
In other words, self-intersection may still happen under different circumstances dpending on the
method used.
*/
class SurfaceHitRefinery final
{
public:
	explicit SurfaceHitRefinery(const SurfaceHit& X);

	/*! @brief Escape this surface in a specific direction using engine settings.
	@param dir The direction to escape. No need to be normalized.
	@return The longest ray in `dir` that avoids this surface.
	*/
	Ray escape(const math::Vector3R& dir) const;

	/*! @brief Escape this surface in a specific direction by a small offset.
	@param dir The direction to escape. No need to be normalized.
	@param delta The amount to offset.
	@return The longest ray in `dir` that avoids this surface.
	*/
	Ray escapeManually(const math::Vector3R& dir, real delta = selfIntersectDelta()) const;

	/*! @brief Escape this surface in a specific direction by some adaptive offset.
	@param dir The direction to escape. No need to be normalized.
	@return The longest ray in `dir` that avoids this surface.
	*/
	Ray escapeEmpirically(const math::Vector3R& dir) const;

	/*! @brief Escape this surface in a specific direction by iteratively re-intersect with the surface.
	This variant is in general the most accurate one but is 5% ~ 15% more expensive than `escapeManually()`.
	@param dir The direction to escape. No need to be normalized.
	@param numIterations The desired number of improvements to have on the offset.
	@return The longest ray in `dir` that avoids this surface.
	*/
	Ray escapeIteratively(
		const math::Vector3R& dir, 
		std::size_t numIterations = numIterations()) const;

	/*! @brief Mutually escape from `X` and `X2` using engine settings.
	@param X2 The other surface to escape from.
	@return The ray that avoids both surfaces.
	*/
	std::optional<Ray> tryEscape(const SurfaceHit& X2) const;

	/*! @brief Mutually escape from `X` and `X2` by a small offset.
	@param X2 The other surface to escape from.
	@param delta The amount to offset.
	@return The ray that avoids both surfaces.
	*/
	std::optional<Ray> tryEscapeManually(const SurfaceHit& X2, real delta = selfIntersectDelta()) const;
	
	/*! @brief Mutually escape from `X` and `X2` by some adaptive offset.
	@param X2 The other surface to escape from.
	@return The ray that avoids both surfaces.
	*/
	std::optional<Ray> tryEscapeEmpirically(const SurfaceHit& X2) const;

	/*! @brief Mutually escape from `X` and `X2` by iteratively re-intersect with the surfaces.
	@param X2 The other surface to escape from.
	@param numIterations The desired number of improvements to have on the offset.
	@return The ray that avoids both surfaces.
	*/
	std::optional<Ray> tryEscapeIteratively(
		const SurfaceHit& X2,
		std::size_t numIterations = numIterations()) const;

public:
	/*! @brief Initialize from engine settings.
	*/
	static void init(const EngineInitSettings& settings);

	/*! @brief A small value for resolving self-intersections.
	*/
	static real selfIntersectDelta();

	/*! @brief Number of iterations to perform when escaping in iterative mode.
	*/
	static std::size_t numIterations();

private:
	struct IterativeOffsetResult
	{
		math::Vector3R offset{0};
		math::Vector3R unitDir{0};
		real maxDistance{0};
	};

	static real fallbackOffsetDist(const SurfaceHit& X, real distanceFactor);
	static real maxErrorOffsetDist(const SurfaceHit& X);
	static real meanErrorOffsetDist(const SurfaceHit& X);
	static math::Vector3R empiricalOffsetVec(const SurfaceHit& X, const math::Vector3R& dir);

	static IterativeOffsetResult iterativeOffset(
		const SurfaceHit& X, 
		const math::Vector3R& dir,
		std::size_t numIterations);

	static IterativeOffsetResult iterativeMutualOffset(
		const SurfaceHit& X,
		const SurfaceHit& X2,
		const math::Vector3R& dir,
		std::size_t numIterations);

	static bool reintersect(const SurfaceHit& X, const Ray& ray, HitProbe& probe);

	/*! Unfortunately we cannot test whether the escape was successful or not for some face
	topologies as the ray can have valid self-intersect against them. This method checks whether
	the specified configuration is one of those cases.
	*/
	static bool canVerifyOffset(const SurfaceHit& X, const math::Vector3R& dir);

	static bool verifyOffset(
		const SurfaceHit& X, 
		const math::Vector3R& dir,
		const math::Vector3R& rayOrigin,
		const math::Vector3R& rayOffset,
		real rayLength = std::numeric_limits<real>::max());

	static ESurfaceRefineMode s_refineMode;
	static real s_selfIntersectDelta;
	static std::size_t s_numIterations;

	const SurfaceHit& m_X;

#if PH_ENABLE_HIT_EVENT_STATS
public:
	static void reportStats();

private:
	struct HitEventStats
	{
		std::atomic_uint64_t numEvents;
		std::atomic_uint64_t numFailedEmpiricalEscapes;
		std::atomic_uint64_t numFailedInitialEscapes;
		std::atomic_uint64_t numFailedIterativeEscapes;
		std::atomic_uint64_t numReintersecs;

		void markEvent(const std::uint64_t num = 1)
		{
			numEvents.fetch_add(num, std::memory_order_relaxed);
		}

		void markFailedEmpiricalEscape()
		{
			numFailedEmpiricalEscapes.fetch_add(1, std::memory_order_relaxed);
		}

		void markFailedInitialEscape()
		{
			numFailedInitialEscapes.fetch_add(1, std::memory_order_relaxed);
		}

		void markFailedIterativeEscape()
		{
			numFailedIterativeEscapes.fetch_add(1, std::memory_order_relaxed);
		}

		void markReintersect()
		{
			numReintersecs.fetch_add(1, std::memory_order_relaxed);
		}
	};

	static HitEventStats s_stats;
#endif
};

inline SurfaceHitRefinery::SurfaceHitRefinery(const SurfaceHit& X)
	: m_X(X)
{}

inline Ray SurfaceHitRefinery::escape(const math::Vector3R& dir) const
{
	PH_ASSERT_MSG(dir.isFinite() && !dir.isZero(), dir.toString());

	switch(s_refineMode)
	{
	case ESurfaceRefineMode::Manual:
		return escapeManually(dir);

	case ESurfaceRefineMode::Empirical:
		return escapeEmpirically(dir);

	case ESurfaceRefineMode::Iterative:
		return escapeIteratively(dir);
	}
	
	PH_ASSERT_UNREACHABLE_SECTION();
	return Ray{};
}

inline Ray SurfaceHitRefinery::escapeManually(const math::Vector3R& dir, const real delta) const
{
#if PH_ENABLE_HIT_EVENT_STATS
	s_stats.markEvent();
#endif

	return Ray(
		m_X.getPosition(),
		dir.normalize(),
		delta,
		std::numeric_limits<real>::max(),
		m_X.getTime());
}

inline Ray SurfaceHitRefinery::escapeEmpirically(const math::Vector3R& dir) const
{
#if PH_ENABLE_HIT_EVENT_STATS
	s_stats.markEvent();
#endif

	return Ray(
		m_X.getPosition() + empiricalOffsetVec(m_X, dir),
		dir.normalize(),
		m_X.getTime());
}

inline Ray SurfaceHitRefinery::escapeIteratively(
	const math::Vector3R& dir,
	const std::size_t numIterations) const
{
#if PH_ENABLE_HIT_EVENT_STATS
	s_stats.markEvent();
#endif

	const IterativeOffsetResult result = iterativeOffset(m_X, dir, numIterations);
	PH_ASSERT_GT(result.maxDistance, 0.0_r);

	return Ray(
		m_X.getPosition() + result.offset,
		result.unitDir,
		0,
		result.maxDistance,
		m_X.getTime());
}

inline std::optional<Ray> SurfaceHitRefinery::tryEscape(const SurfaceHit& X2) const
{
	switch(s_refineMode)
	{
	case ESurfaceRefineMode::Manual:
		return tryEscapeManually(X2);

	case ESurfaceRefineMode::Empirical:
		return tryEscapeEmpirically(X2);

	case ESurfaceRefineMode::Iterative:
		return tryEscapeIteratively(X2);
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return Ray{};
}

inline std::optional<Ray> SurfaceHitRefinery::tryEscapeManually(const SurfaceHit& X2, const real delta) const
{
#if PH_ENABLE_HIT_EVENT_STATS
	s_stats.markEvent(2);
#endif

	const auto xToX2 = X2.getPosition() - m_X.getPosition();
	const auto distance2 = xToX2.lengthSquared();

	// Make sure the two points are distant enough to avoid self-intersection
	// (at least 3 deltas, 2 for ray endpoints and 1 for ray body)
	if(distance2 > math::squared(delta * 3))
	{
		const auto distance = std::sqrt(distance2);
		const auto unitDir = xToX2 / distance;
		return Ray(
			m_X.getPosition(),
			unitDir,
			delta,
			distance - delta,
			m_X.getTime());// following X's time
	}
	else
	{
		return std::nullopt;
	}
}

inline std::optional<Ray> SurfaceHitRefinery::tryEscapeEmpirically(const SurfaceHit& X2) const
{
#if PH_ENABLE_HIT_EVENT_STATS
	s_stats.markEvent(2);
#endif

	const auto xToX2 = X2.getPosition() - m_X.getPosition();
	const auto originX = m_X.getPosition() + empiricalOffsetVec(m_X, xToX2);
	const auto originX2 = X2.getPosition() + empiricalOffsetVec(X2, -xToX2);
	const auto distance = (originX2 - originX).length();
	const auto rcpDistance = 1.0_r / distance;
	if(rcpDistance != 0 && std::isfinite(rcpDistance))
	{
		return Ray(
			originX,
			(originX2 - originX) * rcpDistance,
			0,
			distance,
			m_X.getTime());// following X's time
	}
	else
	{
		return std::nullopt;
	}
}

inline std::optional<Ray> SurfaceHitRefinery::tryEscapeIteratively(
	const SurfaceHit& X2,
	const std::size_t numIterations) const
{
#if PH_ENABLE_HIT_EVENT_STATS
	s_stats.markEvent(2);
#endif

	const auto xToX2 = X2.getPosition() - m_X.getPosition();
	const IterativeOffsetResult result = iterativeMutualOffset(m_X, X2, xToX2, numIterations);
	if(result.maxDistance > 0.0_r && std::isfinite(result.maxDistance))
	{
		return Ray(
			m_X.getPosition() + result.offset,
			result.unitDir,
			0,
			result.maxDistance,
			m_X.getTime());// following X's time
	}
	else
	{
		return std::nullopt;
	}
}

inline real SurfaceHitRefinery::selfIntersectDelta()
{
	return s_selfIntersectDelta;
}

inline std::size_t SurfaceHitRefinery::numIterations()
{
	return s_numIterations;
}

inline real SurfaceHitRefinery::fallbackOffsetDist(const SurfaceHit& X, const real distanceFactor)
{
	const auto fallbackDist = std::max(X.getPosition().length() * distanceFactor, selfIntersectDelta());
	return std::isfinite(fallbackDist) ? fallbackDist : selfIntersectDelta();
}

inline real SurfaceHitRefinery::maxErrorOffsetDist(const SurfaceHit& X)
{
	const auto [_, maxFactor] = X.getDetail().getDistanceErrorFactors();
	const auto offsetDist = X.getPosition().length() * maxFactor;
	if(std::isfinite(offsetDist))
	{
		// Allow to be 0 (if the implementation is confident--with 0 error)
		return offsetDist;
	}
	else
	{
		// A pessimistic mapping obtained from a fairly extreme `IntersectError` test 
		constexpr real distanceFactor = 1e-3_r;
		return fallbackOffsetDist(X, distanceFactor);
	}
}

inline real SurfaceHitRefinery::meanErrorOffsetDist(const SurfaceHit& X)
{
	const auto [meanFactor, _] = X.getDetail().getDistanceErrorFactors();
	const auto offsetDist = X.getPosition().length() * meanFactor;
	if(std::isfinite(offsetDist))
	{
		// Allow to be 0 (if the implementation is confident--with 0 error)
		return offsetDist;
	}
	else
	{
		// A pessimistic mapping obtained from a fairly extreme `IntersectError` test 
		constexpr real distanceFactor = 1e-6_r;
		return fallbackOffsetDist(X, distanceFactor);
	}
}

inline math::Vector3R SurfaceHitRefinery::empiricalOffsetVec(const SurfaceHit& X, const math::Vector3R& dir)
{
	PH_ASSERT_MSG(dir.isFinite() && !dir.isZero(), dir.toString());

	const auto N = X.getGeometryNormal();
	const auto dist = maxErrorOffsetDist(X);
	const auto offsetVec = N.dot(dir) > 0.0_r ? N * dist : N * -dist;

#if PH_ENABLE_HIT_EVENT_STATS
	if(canVerifyOffset(X, dir) && !verifyOffset(X, dir, X.getPosition(), offsetVec))
	{
		s_stats.markFailedEmpiricalEscape();
	}
#endif

	return offsetVec;
}

inline bool SurfaceHitRefinery::reintersect(const SurfaceHit& X, const Ray& ray, HitProbe& probe)
{
#if PH_ENABLE_HIT_EVENT_STATS
	s_stats.markReintersect();
#endif

	return X.reintersect(ray, probe);
}

inline bool SurfaceHitRefinery::canVerifyOffset(const SurfaceHit& X, const math::Vector3R& dir)
{
	const auto N = X.getGeometryNormal();
	const auto topology = X.getDetail().getFaceTopology();

	return topology.hasNo(EFaceTopology::General) &&
	       (topology.hasNo(EFaceTopology::Concave) || N.dot(dir) < 0.0_r) &&
	       (topology.hasNo(EFaceTopology::Convex) || N.dot(dir) > 0.0_r);
}

inline bool SurfaceHitRefinery::verifyOffset(
	const SurfaceHit& X,
	const math::Vector3R& dir,
	const math::Vector3R& rayOrigin,
	const math::Vector3R& rayOffset,
	const real rayLength)
{
	PH_ASSERT(canVerifyOffset(X, dir));

	HitProbe probe;
	Ray ray(rayOrigin + rayOffset, dir.normalize(), 0, rayLength, X.getTime());
	return !X.reintersect(ray, probe);
}

}// end namespace ph::lta
