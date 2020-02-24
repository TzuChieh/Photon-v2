#include "Core/Intersectable/PInfiniteSphere.h"
#include "Common/assertion.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Math/Geometry/TAABB3D.h"
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

	const math::Vector3R hitPos    = ray.getOrigin() + ray.getDirection() * m_effectivelyInfiniteRadius;
	const math::Vector3R hitNormal = ray.getDirection().mul(-1);

	math::Vector3R uvw;
	SphericalMapper().directionToUvw(ray.getDirection(), &uvw);

	out_detail->setMisc(this, uvw, probe.getHitRayT());
	out_detail->getHitInfo(ECoordSys::LOCAL).setAttributes(hitPos, hitNormal, hitNormal);
	out_detail->getHitInfo(ECoordSys::WORLD) = out_detail->getHitInfo(ECoordSys::LOCAL);
}

bool PInfiniteSphere::isIntersectingVolumeConservative(const math::AABB3D& volume) const
{
	return false;
}

math::AABB3D PInfiniteSphere::calcAABB() const
{
	return math::AABB3D(
		math::Vector3R(-std::numeric_limits<real>::infinity()),
		math::Vector3R( std::numeric_limits<real>::infinity()));
}

bool PInfiniteSphere::uvwToPosition(
	const math::Vector3R& uvw,
	const math::Vector3R& observationPoint,
	math::Vector3R* const out_position) const
{
	PH_ASSERT(0.0_r <= uvw.x && uvw.x <= 1.0_r &&
	          0.0_r <= uvw.y && uvw.y <= 1.0_r && 
	          out_position);

	math::Vector3R direction;
	const bool mappingSucceeded = SphericalMapper().uvwToDirection(uvw, &direction);
	PH_ASSERT(mappingSucceeded);

	direction.normalizeLocal();
	*out_position = observationPoint + direction * m_effectivelyInfiniteRadius;
	return true;
}

}// end namespace ph
