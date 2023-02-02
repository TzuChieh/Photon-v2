#include "Core/Intersectable/PLatLongEnvSphere.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Math/Geometry/TSphere.h"
#include "Math/Transform/RigidTransform.h"
#include "Math/Transform/StaticRigidTransform.h"
#include "Math/Geometry/TLineSegment.h"

namespace ph
{

PLatLongEnvSphere::PLatLongEnvSphere(const PrimitiveMetadata* const metadata, const real radius) :
	PLatLongEnvSphere(
		metadata, 
		radius,
		&math::StaticRigidTransform::IDENTITY(),
		&math::StaticRigidTransform::IDENTITY())
{}

PLatLongEnvSphere::PLatLongEnvSphere(
	const PrimitiveMetadata* const    metadata,
	const real                        radius,
	const math::RigidTransform* const localToWorld,
	const math::RigidTransform* const worldToLocal) :

	PBasicSphere(metadata, radius),

	m_localToWorld(localToWorld),
	m_worldToLocal(worldToLocal)
{
	PH_ASSERT(localToWorld);
	PH_ASSERT(worldToLocal);
}

math::Vector2R PLatLongEnvSphere::positionToUV(const math::Vector3R& position) const
{
	// UV is mapped from incident direction for the purpose of environment lighting
	// (no need to renormalize after transform due to rigidity)
	PH_ASSERT(m_worldToLocal);
	math::Vector3R localUnitRayDir;
	m_worldToLocal->transformV(unitRayDir, &localUnitRayDir);
	const math::Vector2R& hitUv = unitSphere.surfaceToLatLong01(localUnitRayDir);
}

// TODO: use exact UV derivatives
void PLatLongEnvSphere::calcIntersectionDetail(
	const Ray&       ray,
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	PH_ASSERT(out_detail);

	const math::Vector3R& hitPosition = ray.getOrigin().add(ray.getDirection().mul(probe.getHitRayT()));
	const math::Vector3R& unitRayDir  = ray.getDirection().normalize();

	// Normal is calculated such that it is always facing the incident ray
	const math::Vector3R& hitNormal = unitRayDir.mul(-1);

	PH_ASSERT_MSG(hitPosition.isFinite() && hitNormal.isFinite(), "\n"
		"hit-position = " + hitPosition.toString() + "\n"
		"hit-normal   = " + hitNormal.toString() + "\n");

	const auto unitSphere = math::TSphere<real>::makeUnit();

	// UV is mapped from incident direction for the purpose of environment lighting
	// (no need to renormalize after transform due to rigidity)
	PH_ASSERT(m_worldToLocal);
	math::Vector3R localUnitRayDir;
	m_worldToLocal->transformV(unitRayDir, &localUnitRayDir);
	const math::Vector2R& hitUv = unitSphere.surfaceToLatLong01(localUnitRayDir);

	out_detail->getHitInfo(ECoordSys::LOCAL).setAttributes(
		hitPosition, 
		hitNormal,
		hitNormal);
	out_detail->getHitInfo(ECoordSys::WORLD) = out_detail->getHitInfo(ECoordSys::LOCAL);
	out_detail->setHitIntrinsics(this, math::Vector3R(hitUv.x(), hitUv.y(), 0), probe.getHitRayT());

	// TODO: derivatives are unset; any point on the sphere can potentially map
	// to any UV for a hemisphere of directions
}

bool PLatLongEnvSphere::latLong01ToSurface(
	const math::Vector2R& latLong01,
	const math::Vector3R& observationPos,
	math::Vector3R* const out_surface) const
{
	PH_ASSERT(m_localToWorld);
	PH_ASSERT(m_worldToLocal);
	PH_ASSERT(out_surface);

	const math::Vector3R& localDir = math::TSphere<real>::makeUnit().latLong01ToSurface(latLong01);

	math::Vector3R localObservationPos;
	m_worldToLocal->transformP(observationPos, &localObservationPos);

	const math::TLineSegment<real> localLine(localObservationPos, localDir);
	const math::TSphere<real> localSphere(getRadius());

	real hitT;
	if(!localSphere.isIntersecting(localLine, &hitT))
	{
		// This means the observer cannot see the environment sphere
		return false;
	}
	const math::Vector3R localHitPos = localLine.getPoint(hitT);

	m_localToWorld->transformP(localHitPos, out_surface);
	return true;
}

}// end namespace ph
