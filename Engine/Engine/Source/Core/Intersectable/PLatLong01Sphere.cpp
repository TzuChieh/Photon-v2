#include "Core/Intersectable/PLatLong01Sphere.h"
#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Math/Geometry/TSphere.h"
#include "Core/SampleGenerator/SampleFlow.h"
#include "Core/Intersectable/Query/PrimitivePosSampleQuery.h"

#include <Common/assertion.h>

namespace ph
{

PLatLong01Sphere::PLatLong01Sphere(const real radius) :
	PBasicSphere(radius)
{}

// TODO: use exact UV derivatives
void PLatLong01Sphere::calcIntersectionDetail(
	const Ray&       ray,
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	PH_ASSERT(out_detail);

	// Refine hit point by normal and radius since the ray can be far away
	// and contains large numerical error
	math::Vector3R hitPosition = ray.getSegment().getPoint(probe.getHitRayT());
	const math::Vector3R& hitNormal = hitPosition.safeNormalize({0, 1, 0});
	hitPosition = hitNormal * getRadius();

	PH_ASSERT_MSG(hitPosition.isFinite() && hitNormal.isFinite(), "\n"
		"hitPosition = " + hitPosition.toString() + "\n"
		"hitNormal   = " + hitNormal.toString() + "\n");

	const math::Vector2R hitUv = positionToUV(hitPosition);

	out_detail->getHitInfo(ECoordSys::Local).setAttributes(
		hitPosition, 
		hitNormal,
		hitNormal);

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

	PH_ASSERT_MSG(dPdU.isFinite() && dPdV.isFinite() &&
	              dNdU.isFinite() && dNdV.isFinite(), "\n"
		"dPdU = " + dPdU.toString() + ", dPdV = " + dPdV.toString() + "\n"
		"dNdU = " + dNdU.toString() + ", dNdV = " + dNdV.toString() + "\n");
}

real PLatLong01Sphere::calcPositionSamplePdfA(const math::Vector3R& position) const
{
	return math::TSphere(getRadius()).uniformSurfaceSamplePdfA();
}

void PLatLong01Sphere::genPositionSample(PrimitivePosSampleQuery& query, SampleFlow& sampleFlow) const
{
	const auto normal = math::TSphere<real>::makeUnit().sampleToSurfaceArchimedes(
		sampleFlow.flow2D());
	const auto position = normal * getRadius();

	query.out.normal = normal;
	query.out.position = position;
	query.out.pdfA = PLatLong01Sphere::calcPositionSamplePdfA(position);

	const math::Vector2R uv = positionToUV(position);
	query.out.uvw = {uv.x(), uv.y(), 0.0_r};
}

math::Vector2R PLatLong01Sphere::positionToUV(const math::Vector3R& position) const
{
	return math::TSphere<real>(getRadius()).surfaceToLatLong01(position);
}

}// end namespace ph
