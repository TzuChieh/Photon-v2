#include "Core/Intersection/PLatLongEnvSphere.h"
#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Math/Geometry/TSphere.h"
#include "Math/Geometry/THemisphere.h"
#include "Math/Geometry/TLineSegment.h"
#include "Math/TOrthonormalBasis3.h"
#include "Math/constant.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/Intersection/Query/PrimitivePosPdfQuery.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>

#include <cmath>

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

void PLatLongEnvSphere::calcHitDetail(
	const Ray&       ray,
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	PH_ASSERT(out_detail);

	probe.popHit();

	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);

	// Refine hit point by normal and radius since the ray can be far away
	// and contains large numerical error
	auto [localHitPosition, localHitNormal] = getRefinedSurfaceAndNormal(
		localRay.getSegment().getPoint(probe.getHitRayT()));

	// Normal is always facing the sphere's center
	localHitNormal *= -1;

	PH_ASSERT_MSG(localHitPosition.isFinite() && localHitNormal.isFinite(), "\n"
		"localHitPosition = " + localHitPosition.toString() + "\n"
		"localHitNormal   = " + localHitNormal.toString() + "\n");

	const auto unitSphere = math::TSphere<real>::makeUnit();
	const auto localUnitRayDir = localRay.getDir().safeNormalize({0, 1, 0});

	// UV is mapped from incident direction for the purpose of environment lighting
	const math::Vector2R hitUV = unitSphere.surfaceToLatLong01(localUnitRayDir);

	math::Vector3R hitPosition;
	m_localToWorld->transformP(localHitPosition, &hitPosition);

	math::Vector3R hitNormal;
	m_localToWorld->transformO(localHitNormal, &hitNormal);

	out_detail->getHitInfo(ECoordSys::Local).setAttributes(
		hitPosition, 
		hitNormal,
		hitNormal);
	out_detail->getHitInfo(ECoordSys::World) = out_detail->getHitInfo(ECoordSys::Local);

	out_detail->setHitIntrinsics(
		this, 
		math::Vector3R(hitUV.x(), hitUV.y(), 0),
		probe.getHitRayT(), 
		HitDetail::NO_FACE_ID, 
		FaceTopology({EFaceTopology::Concave}));

	constexpr auto meanFactor = 5e-8_r;
	out_detail->setDistanceErrorFactors(meanFactor, meanFactor * 1e1_r);

	// Derivatives are unset; any point on the sphere can potentially map to any UV for
	// a hemisphere of directions.
}

void PLatLongEnvSphere::genPosSample(
	PrimitivePosSampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	PH_ASSERT(query.inputs.getUvwPdf().domain == lta::EDomain::UV01);

	if(query.inputs.getObservationPos())
	{
		genPosSampleWithObservationPos(
			{query.inputs.getUvw().x(), query.inputs.getUvw().y()},
			*query.inputs.getUvwPdf(),
			query,
			probe);
	}
	else
	{
		genPosSampleWithoutObservationPos(
			{query.inputs.getUvw().x(), query.inputs.getUvw().y()},
			*query.inputs.getUvwPdf(),
			query,
			sampleFlow,
			probe);
	}
}

void PLatLongEnvSphere::calcPosPdf(PrimitivePosPdfQuery& query) const
{
	if(query.inputs.getObservationPos())
	{
		const math::Vector3R uvw(query.inputs.getUvw());
		const math::Vector2R latLong01(uvw.x(), uvw.y());

		PH_ASSERT(query.inputs.getUvwPdf().domain == lta::EDomain::UV01);
		const real latLong01Pdf = *query.inputs.getUvwPdf();

		calcPosPdfWithObservationPos(latLong01, latLong01Pdf, query);
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

void PLatLongEnvSphere::genPosSampleWithObservationPos(
	const math::Vector2R& latLong01,
	const real latLong01Pdf,
	PrimitivePosSampleQuery& query,
	HitProbe& probe) const
{
	// Observation position must present so one-to-one mapping is possible
	PH_ASSERT(query.inputs.getObservationPos());

	math::Vector3R surface;
	math::Vector3R unitObservationDir;
	if(!latLong01ToSurface(latLong01, *query.inputs.getObservationPos(), &surface, &unitObservationDir))
	{
		return;
	}

	const real sinTheta = std::sin((1.0_r - latLong01.y()) * math::constant::pi<real>);

	// Absolute value of the determinant of Jacobian from UV space to Cartesian
	const real detJacobian = 2.0_r * math::constant::pi2<real> * getRadius() * getRadius() * sinTheta;

	real pdfA = latLong01Pdf / detJacobian;
	pdfA = std::isfinite(pdfA) ? pdfA : 0.0_r;

	const Ray observationRay(
		*query.inputs.getObservationPos(),
		unitObservationDir,
		0,
		getRadius(),
		query.inputs.getTime());

	query.outputs.setPos(surface);
	query.outputs.setObservationRay(observationRay);
	query.outputs.setPdfPos(lta::PDF::A(pdfA));

	probe.pushBaseHit(this, observationRay.getMaxT());
}

void PLatLongEnvSphere::genPosSampleWithoutObservationPos(
	const math::Vector2R& latLong01,
	const real latLong01Pdf,
	PrimitivePosSampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	// Observation position must not present, otherwise it does not make sense to use this
	// method for one-to-many mapping
	PH_ASSERT(!query.inputs.getObservationPos());

	// Current implementation reports the suggested sample direction and PDF no matter if they
	// are requested (`query.suggestDir()`) as they are cheap to compute and is already a by-product.

	math::Vector3R surface;
	math::Vector3R unitObservationDir;
	real pdfA;
	latLong01ToSurface(latLong01, sampleFlow.flow2D(), &surface, &unitObservationDir, &pdfA);

	const real sinTheta = std::sin((1.0_r - latLong01.y()) * math::constant::pi<real>);

	// Absolute value of the determinant of Jacobian from UV space to solid angle
	const real detJacobian = 2.0_r * math::constant::pi2<real> * sinTheta;

	const Ray observationRay(
		surface,
		-unitObservationDir,
		0,
		0,
		query.inputs.getTime());

	query.outputs.setPos(surface);
	query.outputs.setObservationRay(observationRay);
	query.outputs.setPdfPos(lta::PDF::A(pdfA));
	query.outputs.setPdfDir(lta::PDF::W(latLong01Pdf / detJacobian));

	probe.pushBaseHit(this, observationRay.getMaxT());
}

void PLatLongEnvSphere::calcPosPdfWithObservationPos(
	const math::Vector2R& latLong01,
	const real latLong01Pdf,
	PrimitivePosPdfQuery& query) const
{
	const real sinTheta = std::sin((1.0_r - latLong01.y()) * math::constant::pi<real>);

	// Absolute value of the determinant of Jacobian from UV space to Cartesian
	const real detJacobian = 2.0_r * math::constant::pi2<real> * getRadius() * getRadius() * sinTheta;

	query.outputs.setPdf(lta::PDF::A(latLong01Pdf / detJacobian));
}

bool PLatLongEnvSphere::latLong01ToSurface(
	const math::Vector2R& latLong01,
	const math::Vector3R& observationPos,
	math::Vector3R* const out_surface,
	math::Vector3R* const out_unitObservationDir) const
{
	PH_ASSERT(out_surface);
	PH_ASSERT(out_unitObservationDir);

	math::Vector3R localObservationPos;
	m_worldToLocal->transformP(observationPos, &localObservationPos);

	const auto localDir = math::TSphere<real>::makeUnit().latLong01ToSurface(latLong01);
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
	m_localToWorld->transformV(localDir, out_unitObservationDir);
	return true;
}

void PLatLongEnvSphere::latLong01ToSurface(
	const math::Vector2R& latLong01,
	const std::array<real, 2>& uniformSample,
	math::Vector3R* const out_surface,
	math::Vector3R* const out_unitObservationDir,
	real* const out_pdfA) const
{
	PH_ASSERT(out_surface);
	PH_ASSERT(out_unitObservationDir);
	PH_ASSERT(out_pdfA);

	const auto localDir = math::TSphere<real>::makeUnit().latLong01ToSurface(latLong01);
	const auto localDirBasis = math::Basis3R::makeFromUnitY(localDir);

	// In the space local to `localDir`, not in local space
	const math::THemisphere<real> hemisphereInLocalDir(getRadius());

	// We want to uniformly sample all points facing `localDir`, so a cosine-weighted sample
	// is required (so it will be uniform if projected on a disk facing `localDir`). This is
	// slightly different from PBRT-v3's approach [1], as we want to keep the environment
	// sphere's physical size and the points cannot simply be put on a disk.
	const auto surfaceInLocalDir = hemisphereInLocalDir.sampleToSurfaceCosThetaWeighted(
		uniformSample, out_pdfA);

	const auto localSurface = localDirBasis.localToWorld(surfaceInLocalDir);
	m_localToWorld->transformP(localSurface, out_surface);
	m_localToWorld->transformV(localDir, out_unitObservationDir);
}

}// end namespace ph

/*
References:
[1] https://pbr-book.org/3ed-2018/Light_Transport_III_Bidirectional_Methods/The_Path-Space_Measurement_Equation#x2-InfiniteAreaLights
*/
