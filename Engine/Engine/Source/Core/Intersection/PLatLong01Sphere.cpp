#include "Core/Intersection/PLatLong01Sphere.h"
#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Math/Geometry/TSphere.h"
#include "Core/SampleGenerator/SampleFlow.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/Intersection/Query/PrimitivePosSamplePdfQuery.h"

#include <Common/assertion.h>

namespace ph
{

PLatLong01Sphere::PLatLong01Sphere(const real radius) :
	PBasicSphere(radius)
{}

void PLatLong01Sphere::calcHitDetail(
	const Ray&       ray,
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	PH_ASSERT(out_detail);

	// Refine hit point by normal and radius since the ray can be far away
	// and contains large numerical error
	const auto [hitPosition, hitNormal] = getRefinedSurfaceAndNormal(
		ray.getSegment().getPoint(probe.getHitRayT()));

	PH_ASSERT_MSG(hitPosition.isFinite() && hitNormal.isFinite(), "\n"
		"hitPosition = " + hitPosition.toString() + "\n"
		"hitNormal   = " + hitNormal.toString() + "\n");

	const math::Vector2R hitUv = positionToUV(hitPosition);

	out_detail->getHitInfo(ECoordSys::Local).setAttributes(
		hitPosition, 
		hitNormal,
		hitNormal);

	// TODO: use exact UV derivatives; this is finite difference
	const auto [dPdU, dPdV] = math::TSphere<real>(getRadius()).surfaceDerivativesWrtUv(
		hitPosition,
		[this](const math::Vector3R& position)
		{
			return positionToUV(position);
		});

	// Normal derivatives are actually scaled version of dPdU and dPdV
	const math::Vector3R& dNdU = dPdU.mul(getRadius());
	const math::Vector3R& dNdV = dPdV.mul(getRadius());

	out_detail->getHitInfo(ECoordSys::Local).setDerivatives(
		dPdU, dPdV, dNdU, dNdV);

	out_detail->getHitInfo(ECoordSys::World) = out_detail->getHitInfo(ECoordSys::Local);
	out_detail->setHitIntrinsics(
		this, 
		math::Vector3R(hitUv.x(), hitUv.y(), 0), 
		probe.getHitRayT(),
		HitDetail::NO_FACE_ID, 
		FaceTopology({EFaceTopology::Convex}));
	out_detail->resetTransformLevel();

	PH_ASSERT_MSG(dPdU.isFinite() && dPdV.isFinite() &&
	              dNdU.isFinite() && dNdV.isFinite(), "\n"
		"dPdU = " + dPdU.toString() + ", dPdV = " + dPdV.toString() + "\n"
		"dNdU = " + dNdU.toString() + ", dNdV = " + dNdV.toString() + "\n");
}

void PLatLong01Sphere::genPosSample(
	PrimitivePosSampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	const math::TSphere<real> sphere(getRadius());
	const auto pos = sphere.sampleToSurfaceArchimedes(sampleFlow.flow2D());
	const auto pdfA = sphere.uniformSurfaceSamplePdfA();

	const Ray observationRay(
		query.inputs.getObservationPos().value_or(pos),
		pos - query.inputs.getObservationPos().value_or(pos),
		0,
		1,
		query.inputs.getTime());

	query.outputs.setPos(pos);
	query.outputs.setPdfA(pdfA);
	query.outputs.setObservationRay(observationRay);

	probe.pushBaseHit(this, observationRay.getMaxT());
}

void PLatLong01Sphere::calcPosSamplePdfA(
	PrimitivePosSamplePdfQuery& query,
	HitProbe& probe) const
{
	const math::TSphere<real> sphere(getRadius());
	query.outputs.setPdfA(sphere.uniformSurfaceSamplePdfA());

	probe.pushBaseHit(this, query.inputs.getObservationRay().getMaxT());
}

math::Vector2R PLatLong01Sphere::positionToUV(const math::Vector3R& position) const
{
	return math::TSphere<real>(getRadius()).surfaceToLatLong01(position);
}

}// end namespace ph
