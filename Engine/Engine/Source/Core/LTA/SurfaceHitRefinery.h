#pragma once

#include "Core/SurfaceHit.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/math.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cmath>
#include <limits>
#include <optional>

namespace ph { class EngineInitSettings; }

namespace ph::lta
{

class SurfaceHitRefinery final
{
public:
	explicit SurfaceHitRefinery(const SurfaceHit& X);

	Ray escape(const math::Vector3R& dir) const;
	Ray escapeManually(const math::Vector3R& dir, real delta = selfIntersectDelta()) const;
	/*Ray escapeIteratively(const math::Vector3R& dir) const;
	Ray escapePrecisely(const math::Vector3R& dir) const;*/

	std::optional<Ray> tryEscape(const SurfaceHit& X2) const;
	std::optional<Ray> tryEscapeManually(const SurfaceHit& X2, real delta = selfIntersectDelta()) const;

public:
	static void init(const EngineInitSettings& settings);
	static real selfIntersectDelta();

private:
	static real s_selfIntersectDelta;

	const SurfaceHit& m_X;
};

inline SurfaceHitRefinery::SurfaceHitRefinery(const SurfaceHit& X)
	: m_X(X)
{}

inline Ray SurfaceHitRefinery::escape(const math::Vector3R& dir) const
{
	return escapeManually(dir);
}

inline Ray SurfaceHitRefinery::escapeManually(const math::Vector3R& dir, const real delta) const
{
	PH_ASSERT_MSG(dir.isFinite() && !dir.isZero(), dir.toString());

	return Ray(
		m_X.getPosition(),
		dir.normalize(),
		delta,
		std::numeric_limits<real>::max(),
		m_X.getTime());
}

inline std::optional<Ray> SurfaceHitRefinery::tryEscape(const SurfaceHit& X2) const
{
	return tryEscapeManually(X2);
}

inline std::optional<Ray> SurfaceHitRefinery::tryEscapeManually(const SurfaceHit& X2, const real delta) const
{
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

inline real SurfaceHitRefinery::selfIntersectDelta()
{
	return s_selfIntersectDelta;
}

}// end namespace ph::lta
