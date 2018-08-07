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
	const real                     boundingRadius,
	const PrimitiveMetadata* const metadata) :

	Primitive(metadata),

	m_boundingRadius(boundingRadius)
{
	PH_ASSERT(boundingRadius > 0.0_r);
}

bool PInfiniteSphere::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	// Here we assume that <ray> always originated within the bounding radius.
	// If the ray have max-t larger than 2*r, then it must have passed through
	// the imaginary bounding sphere.

	const real rTimes2 = 2.0_r * m_boundingRadius;
	const real rTimes6 = 6.0_r * m_boundingRadius;
	if(ray.getMaxT() > rTimes6)
	{
		probe.pushBaseHit(this, rTimes2);
		return true;
	}
	else
	{
		return false;
	}
}

void PInfiniteSphere::calcIntersectionDetail(
	const Ray&       ray,
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	// FIXME: respect channel mapper

	PH_ASSERT(out_detail);

	const Vector3R hitPos    = ray.getOrigin() + ray.getDirection() * probe.getHitRayT();
	const Vector3R hitNormal = ray.getDirection();

	Vector3R uvw;
	SphericalMapper().map(ray.getDirection(), &uvw);

	out_detail->setMisc(this, uvw, probe.getHitRayT());
	out_detail->getHitInfo(ECoordSys::LOCAL).setAttributes(hitPos, hitNormal, hitNormal);
	out_detail->getHitInfo(ECoordSys::WORLD) = out_detail->getHitInfo(ECoordSys::LOCAL);
}

bool PInfiniteSphere::isIntersectingVolumeConservative(const AABB3D& volume) const
{
	// TODO: assert on aabb is valid

	AABB3D aabb;
	calcAABB(&aabb);
	return aabb.isIntersectingVolume(volume);
}

void PInfiniteSphere::calcAABB(AABB3D* const out_aabb) const
{
	PH_ASSERT(out_aabb);

	// Since we always make intersection point to be 2*r away from ray origin, 
	// this suggests that the bounding box must cover at least [-3*r, 3*r].
	const real rTimes3 = 3.0_r * m_boundingRadius;

	*out_aabb = AABB3D(Vector3R(-rTimes3), Vector3R(rTimes3));
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
	*out_position = m_boundingRadius * 2.0_r * dir;
	return true;
}

}// end namespace ph