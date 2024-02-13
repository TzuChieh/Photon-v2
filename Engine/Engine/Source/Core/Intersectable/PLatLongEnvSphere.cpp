#include "Core/Intersectable/PLatLongEnvSphere.h"
#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Math/Geometry/TSphere.h"
#include "Math/Geometry/TLineSegment.h"
#include "Core/Intersectable/Query/PrimitivePosSampleQuery.h"

#include <Common/assertion.h>

namespace ph
{

PLatLongEnvSphere::PLatLongEnvSphere(const real radius) :
	PLatLongEnvSphere(
		radius,
		&math::StaticRigidTransform::IDENTITY(),
		&math::StaticRigidTransform::IDENTITY())
{}

PLatLongEnvSphere::PLatLongEnvSphere(
	const real radius,
	const math::StaticRigidTransform* const localToWorld,
	const math::StaticRigidTransform* const worldToLocal)

	: PBasicSphere(radius)

	, m_localToWorld(localToWorld)
	, m_worldToLocal(worldToLocal)
	, m_worldOrigin()
{
	PH_ASSERT(localToWorld);
	PH_ASSERT(worldToLocal);

	m_localToWorld->transformP({0, 0, 0}, &m_worldOrigin);
}

bool PLatLongEnvSphere::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);
	return PBasicSphere::isIntersecting(localRay, probe);
}

bool PLatLongEnvSphere::isOccluding(const Ray& ray) const
{
	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);
	return PBasicSphere::isOccluding(localRay);
}

// TODO: use exact UV derivatives
void PLatLongEnvSphere::calcIntersectionDetail(
	const Ray&       ray,
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	PH_ASSERT(out_detail);

	// Refine hit point by normal and radius since the ray can be far away
	// and contains large numerical error
	math::Vector3R hitPosition = ray.getSegment().getPoint(probe.getHitRayT());
	hitPosition = hitPosition.safeNormalize({0, 1, 0}) * getRadius();

	// Normal is calculated such that it is always facing the incident ray
	const math::Vector3R& unitRayDir  = ray.getDirection().normalize();
	const math::Vector3R& hitNormal = unitRayDir.mul(-1);

	PH_ASSERT_MSG(hitPosition.isFinite() && hitNormal.isFinite(), "\n"
		"hitPosition = " + hitPosition.toString() + "\n"
		"hitNormal   = " + hitNormal.toString() + "\n");

	const auto unitSphere = math::TSphere<real>::makeUnit();

	// UV is mapped from incident direction for the purpose of environment lighting
	// (no need to renormalize after transform due to rigidity)
	PH_ASSERT(m_worldToLocal);
	math::Vector3R localUnitRayDir;
	m_worldToLocal->transformV(unitRayDir, &localUnitRayDir);
	const math::Vector2R& hitUv = unitSphere.surfaceToLatLong01(localUnitRayDir);

	out_detail->getHitInfo(ECoordSys::Local).setAttributes(
		hitPosition, 
		hitNormal,
		hitNormal);
	out_detail->getHitInfo(ECoordSys::World) = out_detail->getHitInfo(ECoordSys::Local);
	out_detail->setHitIntrinsics(
		this, 
		math::Vector3R(hitUv.x(), hitUv.y(), 0), 
		probe.getHitRayT(), 
		HitDetail::NO_FACE_ID, 
		FaceTopology({EFaceTopology::Concave}));

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

	math::Vector3R localObservationPos;
	m_worldToLocal->transformP(observationPos, &localObservationPos);

	const math::Vector3R localDir = math::TSphere<real>::makeUnit().latLong01ToSurface(latLong01);
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
