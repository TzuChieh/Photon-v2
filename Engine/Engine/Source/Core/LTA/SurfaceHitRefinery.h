#pragma once

#include "Core/SurfaceHit.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/math.h"
#include "ESurfaceRefineMode.h"
#include "Core/LTA/SidednessAgreement.h"

#include <Common/config.h>
#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <cmath>
#include <limits>
#include <optional>
#include <atomic>

namespace ph { class EngineInitSettings; }

namespace ph::lta
{

class SurfaceHitRefinery final
{
public:
	explicit SurfaceHitRefinery(const SurfaceHit& X);

	Ray escape(const math::Vector3R& dir) const;
	Ray escapeManually(const math::Vector3R& dir, real delta = selfIntersectDelta()) const;
	Ray escapeEmpirically(const math::Vector3R& dir) const;

	Ray escapeIteratively(
		const math::Vector3R& dir, 
		std::size_t numIterations = 2) const;

	std::optional<Ray> tryEscape(const SurfaceHit& X2) const;
	std::optional<Ray> tryEscapeManually(const SurfaceHit& X2, real delta = selfIntersectDelta()) const;
	std::optional<Ray> tryEscapeEmpirically(const SurfaceHit& X2) const;

	std::optional<Ray> tryEscapeIteratively(
		const SurfaceHit& X2,
		std::size_t numIterations = 2) const;

public:
	static void init(const EngineInitSettings& settings);
	static real selfIntersectDelta();
	static real empiricalOffsetDist(const SurfaceHit& X);
	static math::Vector3R empiricalOffsetVec(const SurfaceHit& X, const math::Vector3R& dir);

	static math::Vector3R iterativeOffsetVec(
		const SurfaceHit& X, 
		const math::Vector3R& dir,
		std::size_t numIterations);

private:
	static ESurfaceRefineMode s_refineMode;
	static real s_selfIntersectDelta;

	const SurfaceHit& m_X;

#if PH_ENABLE_HIT_EVENT_STATS
public:
	static void reportStats();

private:
	struct HitEventStats
	{
		std::atomic_uint64_t numEvents;
		std::atomic_uint64_t numFailedEmpiricalEscapes;
		std::atomic_uint64_t numIterations;

		void markEvent()
		{
			numEvents.fetch_add(1, std::memory_order_relaxed);
		}

		void markFailedEmpiricalEscape()
		{
			numFailedEmpiricalEscapes.fetch_add(1, std::memory_order_relaxed);
		}

		void markIteration()
		{
			numIterations.fetch_add(1, std::memory_order_relaxed);
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
	PH_ASSERT_MSG(dir.isFinite() && !dir.isZero(), dir.toString());

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
		0,
		std::numeric_limits<real>::max(),
		m_X.getTime());
}

inline Ray SurfaceHitRefinery::escapeIteratively(
	const math::Vector3R& dir,
	const std::size_t numIterations) const
{
#if PH_ENABLE_HIT_EVENT_STATS
	s_stats.markEvent();
#endif

	return Ray(
		m_X.getPosition() + iterativeOffsetVec(m_X, dir, numIterations),
		dir.normalize(),
		0,
		std::numeric_limits<real>::max(),
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
	s_stats.markEvent();
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
	s_stats.markEvent();
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
	s_stats.markEvent();
#endif

	const auto xToX2 = X2.getPosition() - m_X.getPosition();
	const auto originX = m_X.getPosition() + iterativeOffsetVec(m_X, xToX2, numIterations);
	const auto originX2 = X2.getPosition() + iterativeOffsetVec(X2, -xToX2, numIterations);
	const auto distance = (originX2 - originX).length();
	const auto rcpDistance = 1.0_r / distance;
	if(rcpDistance != 0 && std::isfinite(rcpDistance))
	{
		// Mutual escape like this changes the originally analyzed ray directions and lengths.
		// This can defeat the efforts done by `iterativeOffsetVec()` in some cases.
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

inline real SurfaceHitRefinery::selfIntersectDelta()
{
	return s_selfIntersectDelta;
}

inline real SurfaceHitRefinery::empiricalOffsetDist(const SurfaceHit& X)
{
	const auto dist = X.getPosition().length() * 1e-3_r;
	return dist > 0.0_r && std::isfinite(dist) ? dist : selfIntersectDelta();
}

inline math::Vector3R SurfaceHitRefinery::empiricalOffsetVec(const SurfaceHit& X, const math::Vector3R& dir)
{
	PH_ASSERT_MSG(dir.isFinite() && !dir.isZero(), dir.toString());

	const auto dist = empiricalOffsetDist(X);
	return SidednessAgreement{ESidednessPolicy::TrustGeometry}.isFrontHemisphere(X, dir)
		? X.getGeometryNormal() * dist
		: X.getGeometryNormal() * -dist;
}

}// end namespace ph::lta
