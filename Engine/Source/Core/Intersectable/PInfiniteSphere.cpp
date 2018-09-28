#include "Core/Intersectable/PInfiniteSphere.h"
#include "Common/assertion.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/Bound/AABB3D.h"
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
	Vector3R* const out_position) const
{
	PH_ASSERT(0.0_r <= uvw.x && uvw.x <= 1.0_r &&
	          0.0_r <= uvw.y && uvw.y <= 1.0_r && 
	          out_position);

	const real theta = (1.0_r - uvw.y) * PH_PI_REAL;
	const real phi   = uvw.x * PH_PI_REAL * 2.0_r;

	const real zxPlaneRadius = std::sin(theta);
	const Vector3R dir(zxPlaneRadius * std::sin(phi),
	                   std::cos(theta),
	                   zxPlaneRadius * std::cos(phi));
	*out_position = m_effectivelyInfiniteRadius * 2.0_r * dir;
	return SphericalMapper().u;
}

}// end namespace ph