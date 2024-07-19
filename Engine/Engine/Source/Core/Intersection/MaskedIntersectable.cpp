#include "Core/Intersection/MaskedIntersectable.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Core/SurfaceHit.h"
#include "Core/Texture/TSampler.h"
#include "Math/hash.h"
#include "Math/Random/sample.h"
#include "Core/LTA/SurfaceHitRefinery.h"

#include <Common/assertion.h>

#include <array>
#include <cmath>
#include <limits>
#include <optional>

namespace ph
{

namespace
{

inline SurfaceHit report_hit(const Ray& ray, const HitProbe& probe)
{
	// We do not need to compute bases for sampling the mask
	HitDetail detail;
	probe.calcHitDetail(ray, &detail);
	return SurfaceHit(ray, probe, detail, SurfaceHitReason(ESurfaceHitReason::IncidentRay));
}

inline std::optional<Ray> next_ray_from(const SurfaceHit& X)
{
	if(X.getDetail().getFaceTopology().has(EFaceTopology::Planar))
	{
		return std::nullopt;
	}

	const Ray& mainRay = X.getRay();

	// For non-planar local topologies (e.g., a sphere), a further hit is still possible if
	// current hit is masked off. We need to search for potential next hit iteratively by forming
	// a new ray from `X`:
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

}// end anonymous namespace

MaskedIntersectable::MaskedIntersectable(
	const Intersectable* intersectable,
	const std::shared_ptr<TTexture<real>>& mask,
	const uint8 maxIterations)

	: Intersectable()

	, m_intersectable(intersectable)
	, m_mask(mask)
	, m_maxIterations(maxIterations)
{
	PH_ASSERT(intersectable);
	PH_ASSERT(mask);
}

bool MaskedIntersectable::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	Ray nextRay = ray;
	HitProbe nextProbe = probe;
	for(uint8 numIterations = 0; numIterations < m_maxIterations; ++numIterations)
	{
		if(!m_intersectable->isIntersecting(nextRay, nextProbe))
		{
			break;
		}

		const SurfaceHit X(report_hit(nextRay, nextProbe));
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
			const auto optNextRay = next_ray_from(X);
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

bool MaskedIntersectable::reintersect(
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
	for(uint8 numIterations = 0; numIterations < m_maxIterations; ++numIterations)
	{
		if(!srcProbe.getTopHit()->reintersect(nextRay, nextProbe, srcRay, srcProbe))
		{
			return false;
		}

		const SurfaceHit X(report_hit(nextRay, nextProbe));
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
			const auto optNextRay = next_ray_from(X);
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

void MaskedIntersectable::calcHitDetail(
	const Ray& ray,
	HitProbe& probe,
	HitDetail* const out_detail) const
{
	PH_ASSERT(probe.getTopHit() == this);
	probe.popHit();

	PH_ASSERT(probe.getTopHit());
	probe.getTopHit()->calcHitDetail(ray, probe, out_detail);
}

math::AABB3D MaskedIntersectable::calcAABB() const
{
	return m_intersectable->calcAABB();
}

bool MaskedIntersectable::isOnMask(const SurfaceHit& X) const
{
	const real maskValue = TSampler<real>{}.sample(*m_mask, X);
	PH_ASSERT_IN_RANGE_INCLUSIVE(maskValue, 0.0_r, 1.0_r);

	const std::array<math::Vector3R, 2> deterministicEntropySource = {
		X.getRay().getOrigin(), X.getRay().getDir()};

	// Decide whether the surface hit is under mask by a deterministic sample (also known as
	// hashed alpha testing, see Chris Wayman and Morgan McGuire's I3D paper in 2017 and PBRT-v4).
	const auto sample = math::bits_to_sample<real>(math::murmur3_32(deterministicEntropySource, 0));
	return math::pick(maskValue, sample);
}

}// end namespace ph
