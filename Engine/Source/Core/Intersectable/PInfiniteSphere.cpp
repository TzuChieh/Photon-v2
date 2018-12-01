#include "Core/Intersectable/PInfiniteSphere.h"
#include "Common/assertion.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/Bound/TAABB3D.h"
#include "Math/TVector3.h"
#include "Core/Sample/PositionSample.h"
#include "Core/Intersectable/UvwMapper/SphericalMapper.h"
#include "Math/constant.h"

#include <limits>
#include <cmath>

namespace ph
{

PInfiniteSphere::PInfiniteSphere(
	const real effectivelyInfiniteRadius,
	const PrimitiveMetadata* const metadata) :

	Primitive(metadata),

	m_effectivelyInfiniteRadius(effectivelyInfiniteRadius)
{
	PH_ASSERT(std::isnormal(effectivelyInfiniteRadius));
}

bool PInfiniteSphere::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	if(ray.getMaxT() >= std::numeric_limits<real>::max())
	{
		probe.pushBaseHit(this, ray.getMaxT());
		return true;
	}

	return false;
}

bool PInfiniteSphere::isIntersecting(const Ray& ray) const
{
	return ray.getMaxT() >= std::numeric_limits<real>::max();
}

void PInfiniteSphere::calcIntersectionDetail(
	const Ray&       ray,
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	// FIXME: respect channel mapper

	PH_ASSERT(out_detail);

	const Vector3R hitPos    = ray.getOrigin() + ray.getDirection() * m_effectivelyInfiniteRadius;
	const Vector3R hitNormal = ray.getDirection().mul(-1);

	Vector3R uvw;
	SphericalMapper().directionToUvw(ray.getDirection(), &uvw);

	out_detail->setMisc(this, uvw, probe.getHitRayT());
	out_detail->getHitInfo(ECoordSys::LOCAL).setAttributes(hitPos, hitNormal, hitNormal);
	out_detail->getHitInfo(ECoordSys::WORLD) = out_detail->getHitInfo(ECoordSys::LOCAL);
}

bool PInfiniteSphere::isIntersectingVolumeConservative(const AABB3D& volume) const
{
	return false;
}

void PInfiniteSphere::calcAABB(AABB3D* const out_aabb) const
{
	PH_ASSERT(out_aabb);

	*out_aabb = AABB3D(Vector3R(-std::numeric_limits<real>::infinity()), 
	                   Vector3R( std::numeric_limits<real>::infinity()));
}

bool PInfiniteSphere::uvwToPosition(
	const Vector3R& uvw,
	const Vector3R& observationPoint,
	Vector3R* const out_position) const
{
	PH_ASSERT(0.0_r <= uvw.x && uvw.x <= 1.0_r &&
	          0.0_r <= uvw.y && uvw.y <= 1.0_r && 
	          out_position);

	Vector3R direction;
	const bool mappingSucceeded = SphericalMapper().uvwToDirection(uvw, &direction);
	PH_ASSERT(mappingSucceeded);

	direction.normalizeLocal();
	*out_position = observationPoint + direction * m_effectivelyInfiniteRadius;
	return true;
}

}// end namespace ph