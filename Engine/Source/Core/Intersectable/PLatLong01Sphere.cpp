#include "Core/Intersectable/PLatLong01Sphere.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Math/Geometry/TSphere.h"

namespace ph
{

PLatLong01Sphere::PLatLong01Sphere(const PrimitiveMetadata* const metadata, const real radius) :
	PBasicSphere(metadata, radius)
{}

// TODO: use exact UV derivatives
void PLatLong01Sphere::calcIntersectionDetail(
	const Ray&       ray,
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	PH_ASSERT(out_detail);

	const math::Vector3R& hitPosition = ray.getOrigin().add(ray.getDirection().mul(probe.getHitRayT()));
	const math::Vector3R& hitNormal   = hitPosition.normalize();

	PH_ASSERT_MSG(hitPosition.isFinite() && hitNormal.isFinite(), "\n"
		"hit-position = " + hitPosition.toString() + "\n"
		"hit-normal   = " + hitNormal.toString() + "\n");

	const math::TSphere<real> sphere(getRadius());

	const math::Vector2R& hitUv = sphere.surfaceToLatLong01(hitPosition);

	out_detail->getHitInfo(ECoordSys::LOCAL).setAttributes(
		hitPosition, 
		hitNormal,
		hitNormal);

	const auto [dPdU, dPdV] = sphere.surfaceDerivativesWrtUv(
		hitPosition,
		[sphere](const math::Vector3R& position)
		{
			return sphere.surfaceToLatLong01(position);
		});

	// Normal derivatives are actually scaled version of dPdU and dPdV
	const math::Vector3R& dNdU = dPdU.mul(getRadius());
	const math::Vector3R& dNdV = dPdV.mul(getRadius());

	out_detail->getHitInfo(ECoordSys::LOCAL).setDerivatives(
		dPdU, dPdV, dNdU, dNdV);

	out_detail->getHitInfo(ECoordSys::WORLD) = out_detail->getHitInfo(ECoordSys::LOCAL);
	out_detail->setMisc(this, math::Vector3R(hitUv.x(), hitUv.y(), 0), probe.getHitRayT());

	PH_ASSERT_MSG(dPdU.isFinite() && dPdV.isFinite() &&
	              dNdU.isFinite() && dNdV.isFinite(), "\n"
		"dPdU = " + dPdU.toString() + ", dPdV = " + dPdV.toString() + "\n"
		"dNdU = " + dNdU.toString() + ", dNdV = " + dNdV.toString() + "\n");
}

}// end namespace ph
