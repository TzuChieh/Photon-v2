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

namespace
{
	constexpr real EFFECTIVELY_INFINITE_RADIUS = 1e30_r;
}

PInfiniteSphere::PInfiniteSphere(const PrimitiveMetadata* const metadata) : 
	Primitive(metadata)
{}

bool PInfiniteSphere::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	// assuming ray origin can be approximated as (0, 0, 0) given this sphere
	// is infinitely large
	if(ray.getMinT() < EFFECTIVELY_INFINITE_RADIUS && EFFECTIVELY_INFINITE_RADIUS < ray.getMaxT())
	{
		probe.pushBaseHit(this, EFFECTIVELY_INFINITE_RADIUS);
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

	// assuming ray origin can be approximated as (0, 0, 0) given this sphere
	// is infinitely large
	const Vector3R hitPos    = ray.getDirection() * probe.getHitRayT();
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

	*out_aabb = AABB3D(Vector3R(-EFFECTIVELY_INFINITE_RADIUS), 
	                   Vector3R( EFFECTIVELY_INFINITE_RADIUS));
}

Vector3R PInfiniteSphere::uvwToPosition(const Vector3R& uvw) const
{
	PH_ASSERT(0.0_r <= uvw.x && uvw.x <= 1.0_r &&
	          0.0_r <= uvw.y && uvw.y <= 1.0_r);

	const real theta = uvw.y * PH_PI_REAL;
	const real phi   = uvw.x * PH_PI_REAL * 2.0_r;

	const real zxPlaneRadius = std::sin(theta);
	const Vector3R dir(zxPlaneRadius * std::sin(phi),
	                   std::cos(theta),
	                   zxPlaneRadius * std::cos(phi));
	return dir.mul(EFFECTIVELY_INFINITE_RADIUS);
}

}// end namespace ph