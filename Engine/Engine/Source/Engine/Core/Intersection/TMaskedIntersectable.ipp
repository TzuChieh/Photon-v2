#pragma once

#include "Engine/Core/Intersection/TMaskedIntersectable.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/HitProbe.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/LTA/SurfaceHitRefinery.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Texture/TSampler.h"
#include "Engine/Math/Random/sample.h"
#include "Engine/Math/hash.h"

#include <Common/assertion.h>

#include <array>
#include <cmath>
#include <limits>
#include <optional>
#include <utility>

namespace ph
{

inline real MaterialInterfaceMask::operator () (const SurfaceHit& X) const
{
	const TTexture<real>* const mask = X.getMetadata().getInterfaceMask();
	return mask ? TSampler<real>{}.sample(*mask, X) : 1.0_r;
}

namespace detail::masked_intersectable
{

inline SurfaceHit report_hit(const Ray& ray, const HitProbe& probe)
{
	// We do not need to compute bases for sampling the mask
	HitDetail detail;
	probe.calcHitDetail(ray, &detail);
	return SurfaceHit(
		ray,
		probe,
		detail,
		&SurfaceHit::getPrimitiveMetadataRef(detail),
		ESurfaceHitReason::IncidentRay,
		false);
}

inline std::optional<Ray> next_ray_from(const SurfaceHit& X)
{
	const Ray& mainRay = X.getRay();

	// For non-planar local topologies (e.g., a sphere), a further hit is still possible if
	// current hit is masked off. This is also true for a planar face in an aggregate (e.g., a
	// mesh), as local face topology does not describe a group of intersectables. We need to
	// search for potential next hits iteratively by forming a new ray from `X`:
	Ray nextRay = lta::SurfaceHitRefinery{X}.escape(mainRay.getDir());
	real nextRayMaxT = nextRay.getSegment().getProjectedT(mainRay.getHead());
	nextRayMaxT = std::isfinite(nextRayMaxT) ? nextRayMaxT : std::numeric_limits<real>::max();
	if(nextRayMaxT <= nextRay.getMinT())
	{
		return std::nullopt;
	}

	nextRay.setMaxT(nextRayMaxT);
	return nextRay;

	// Note: It is unfortunate that the use of `getProjectedT()` here and in the implementation
	// of `MaskedIntersectable` can cause infinite loop for non-planar shapes sometimes.
	// `m_maxIterations` can guard against it though.

	// TODO: assert rare for high number of loops; and if it is too often we need to investigate
}

}// end namespace detail::masked_intersectable

template<typename Mask>
inline TMaskedIntersectable<Mask>::TMaskedIntersectable(
	const Intersectable* const intersectable,
	Mask mask,
	const uint32 maxIterations)

	: Intersectable()

	, m_intersectable(intersectable)
	, m_mask(std::move(mask))
	, m_maxIterations(maxIterations)
{
	PH_ASSERT(intersectable);
}

template<typename Mask>
inline bool TMaskedIntersectable<Mask>::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	Ray nextRay = ray;
	HitProbe nextProbe = probe;
	for(uint32 numIterations = 0; numIterations < m_maxIterations; ++numIterations)
	{
		if(!m_intersectable->isIntersecting(nextRay, nextProbe))
		{
			break;
		}

		const SurfaceHit X(detail::masked_intersectable::report_hit(nextRay, nextProbe));
		if(isOnMask(X))
		{
			const auto hitPos = nextRay.getSegment().getPoint(nextProbe.getHitRayT());
			const real hitT = ray.getSegment().getProjectedT(hitPos);

			probe = nextProbe;
			probe.replaceBaseHitRayT(hitT);
			probe.pushIntermediateHit(this);
			return true;
		}
		else if(numIterations + 1 < m_maxIterations)
		{
			const auto optNextRay = detail::masked_intersectable::next_ray_from(X);
			if(!optNextRay)
			{
				break;
			}

			nextRay = *optNextRay;
			nextProbe = probe;
		}
	}

	return false;
}

template<typename Mask>
inline bool TMaskedIntersectable<Mask>::reintersect(
	const Ray& ray,
	HitProbe& probe,
	const Ray& srcRay,
	HitProbe& srcProbe) const
{
	PH_ASSERT(srcProbe.getTopHit() == this);
	srcProbe.popHit();

	// May need to restore the state of `srcProbe` later
	const HitProbe srcProbeRecord = srcProbe;

	Ray nextRay = ray;
	HitProbe nextProbe = probe;
	for(uint32 numIterations = 0; numIterations < m_maxIterations; ++numIterations)
	{
		if(!srcProbe.getTopHit()->reintersect(nextRay, nextProbe, srcRay, srcProbe))
		{
			return false;
		}

		const SurfaceHit X(detail::masked_intersectable::report_hit(nextRay, nextProbe));
		if(isOnMask(X))
		{
			const auto hitPos = nextRay.getSegment().getPoint(nextProbe.getHitRayT());
			const real hitT = ray.getSegment().getProjectedT(hitPos);

			probe = nextProbe;
			probe.replaceBaseHitRayT(hitT);
			probe.pushIntermediateHit(this);
			return true;
		}
		else if(numIterations + 1 < m_maxIterations)
		{
			const auto optNextRay = detail::masked_intersectable::next_ray_from(X);
			if(!optNextRay)
			{
				break;
			}

			nextRay = *optNextRay;
			nextProbe = probe;
			srcProbe = srcProbeRecord;
		}
	}

	return false;
}

template<typename Mask>
inline void TMaskedIntersectable<Mask>::calcHitDetail(
	const Ray& ray,
	HitProbe& probe,
	HitDetail* const out_detail) const
{
	PH_ASSERT(probe.getTopHit() == this);
	probe.popHit();

	PH_ASSERT(probe.getTopHit());
	probe.getTopHit()->calcHitDetail(ray, probe, out_detail);
}

template<typename Mask>
inline math::AABB3D TMaskedIntersectable<Mask>::calcAABB() const
{
	return m_intersectable->calcAABB();
}

template<typename Mask>
inline bool TMaskedIntersectable<Mask>::isOnMask(const SurfaceHit& X) const
{
	const real maskValue = m_mask(X);
	PH_ASSERT_IN_RANGE_INCLUSIVE(maskValue, 0.0_r, 1.0_r);
	if(maskValue == 1.0_r || maskValue == 0.0_r)
	{
		return maskValue == 1.0_r;
	}

	const std::array<math::Vector3R, 2> deterministicEntropySource = {
		X.getRay().getOrigin(), X.getRay().getDir()};

	// Decide whether the surface hit is under mask by a deterministic sample (also known as
	// hashed alpha testing, see Chris Wayman and Morgan McGuire's I3D paper in 2017 and PBRT-v4).
	const auto sample = math::bits_to_sample<real>(math::murmur3_32(deterministicEntropySource, 0));
	return math::pick(maskValue, sample);
}

}// end namespace ph
