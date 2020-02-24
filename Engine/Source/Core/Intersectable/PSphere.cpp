#include "Core/Intersectable/PSphere.h"
#include "Common/assertion.h"
#include "Math/constant.h"
#include "Math/Geometry/TAABB3D.h"
#include "Math/math.h"
#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/UvwMapper/UvwMapper.h"
#include "Math/TMatrix2.h"
#include "Core/Sample/PositionSample.h"
#include "Math/TOrthonormalBasis3.h"
#include "Math/Geometry/TSphere.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace ph
{

PSphere::PSphere(const PrimitiveMetadata* const metadata, const real radius) : 
	Primitive(metadata),
	m_radius(radius), m_reciRadius(radius > 0.0_r ? 1.0_r / radius : 0.0_r)
{
	PH_ASSERT_GE(radius, 0.0_r);
}

bool PSphere::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	real hitT;
	if(!math::TSphere(m_radius).isIntersecting(ray.getSegment(), &hitT))
	{
		return false;
	}

	probe.pushBaseHit(this, hitT);
	return true;
}

void PSphere::calcIntersectionDetail(
	const Ray&       ray, 
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	PH_ASSERT(out_detail);
	PH_ASSERT(m_metadata);

	const UvwMapper* mapper = m_metadata->getChannel(probe.getChannel()).getMapper();

	const math::Vector3R& hitPosition = ray.getOrigin().add(ray.getDirection().mul(probe.getHitRayT()));
	const math::Vector3R& hitNormal   = hitPosition.normalize();

	PH_ASSERT_MSG(hitPosition.isFinite() && hitNormal.isFinite(), "\n"
		"hit-position = " + hitPosition.toString() + "\n"
		"hit-normal   = " + hitNormal.toString() + "\n");

	PH_ASSERT(mapper);
	math::Vector3R hitUvw;
	mapper->positionToUvw(hitPosition, &hitUvw);

	out_detail->getHitInfo(ECoordSys::LOCAL).setAttributes(
		hitPosition, 
		hitNormal,
		hitNormal);

	// compute partial derivatives using 2nd-order approximation

	// calculating displacement vectors on hit normal's tangent plane
	// FIXME: hard-coded number
	const real delta = m_radius / 128.0_r;
	
	const auto hitBasis = math::Basis3R::makeFromUnitY(hitNormal);
	const math::Vector3R dx = hitBasis.getXAxis().mul(delta);
	const math::Vector3R dz = hitBasis.getZAxis().mul(delta);

	// find delta positions on the sphere from displacement vectors
	const math::Vector3R negX = hitPosition.sub(dx).normalizeLocal().mulLocal(m_radius);
	const math::Vector3R posX = hitPosition.add(dx).normalizeLocal().mulLocal(m_radius);
	const math::Vector3R negZ = hitPosition.sub(dz).normalizeLocal().mulLocal(m_radius);
	const math::Vector3R posZ = hitPosition.add(dz).normalizeLocal().mulLocal(m_radius);

	// find delta uvw vectors
	math::Vector3R negXuvw, posXuvw, negZuvw, posZuvw;
	mapper->positionToUvw(negX, &negXuvw);
	mapper->positionToUvw(posX, &posXuvw);
	mapper->positionToUvw(negZ, &negZuvw);
	mapper->positionToUvw(posZ, &posZuvw);

	// calculate positional partial derivatives
	math::Vector3R dPdU, dPdV;
	const math::Matrix2R uvwDiff(
		posXuvw.x - negXuvw.x, posXuvw.y - negXuvw.y,
		posZuvw.x - negZuvw.x, posZuvw.y - negZuvw.y);
	if(!uvwDiff.solve(posX.sub(negX), posZ.sub(negZ), &dPdU, &dPdV))
	{
		const auto uvwBasis = math::Basis3R::makeFromUnitY(hitNormal);
		dPdU = uvwBasis.getXAxis();
		dPdV = uvwBasis.getXAxis();
	}

	// normal derivatives are actually scaled version of dPdU and dPdV
	const math::Vector3R& dNdU = dPdU.mul(m_reciRadius);
	const math::Vector3R& dNdV = dPdV.mul(m_reciRadius);

	out_detail->getHitInfo(ECoordSys::LOCAL).setDerivatives(
		dPdU, dPdV, dNdU, dNdV);

	out_detail->getHitInfo(ECoordSys::WORLD) = out_detail->getHitInfo(ECoordSys::LOCAL);
	out_detail->setMisc(this, hitUvw, probe.getHitRayT());

	PH_ASSERT_MSG(dPdU.isFinite() && dPdV.isFinite() &&
	              dNdU.isFinite() && dNdV.isFinite(), "\n"
		"dPdU = " + dPdU.toString() + ", dPdV = " + dPdV.toString() + "\n"
		"dNdU = " + dNdU.toString() + ", dNdV = " + dNdV.toString() + "\n");
}

// Intersection test for solid box and hollow sphere.
// Reference: Jim Arvo's algorithm in Graphics Gems 2
bool PSphere::isIntersectingVolumeConservative(const math::AABB3D& volume) const
{
	const real radius2 = math::squared(m_radius);

	// These variables are gonna store minimum and maximum squared distances 
	// from the sphere's center to the AABB volume.
	real minDist2 = 0.0_r;
	real maxDist2 = 0.0_r;

	real a, b;

	a = math::squared(volume.getMinVertex().x);
	b = math::squared(volume.getMaxVertex().x);
	maxDist2 += std::max(a, b);
	if     (0.0_r < volume.getMinVertex().x) minDist2 += a;
	else if(0.0_r > volume.getMaxVertex().x) minDist2 += b;

	a = math::squared(volume.getMinVertex().y);
	b = math::squared(volume.getMaxVertex().y);
	maxDist2 += std::max(a, b);
	if     (0.0_r < volume.getMinVertex().y) minDist2 += a;
	else if(0.0_r > volume.getMaxVertex().y) minDist2 += b;

	a = math::squared(volume.getMinVertex().z);
	b = math::squared(volume.getMaxVertex().z);
	maxDist2 += std::max(a, b);
	if     (0.0_r < volume.getMinVertex().z) minDist2 += a;
	else if(0.0_r > volume.getMaxVertex().z) minDist2 += b;

	return minDist2 <= radius2 && radius2 <= maxDist2;
}

math::AABB3D PSphere::calcAABB() const
{
	return math::AABB3D(
		math::Vector3R(-m_radius, -m_radius, -m_radius),
		math::Vector3R( m_radius,  m_radius,  m_radius)).
		expand(math::Vector3R(0.0001_r * m_radius));
}

real PSphere::calcPositionSamplePdfA(const math::Vector3R& position) const
{
	return 1.0_r / this->PSphere::calcExtendedArea();
}

void PSphere::genPositionSample(SampleFlow& sampleFlow, PositionSample* const out_sample) const
{
	PH_ASSERT(out_sample);
	PH_ASSERT(m_metadata);

	out_sample->normal = math::TSphere<real>::makeUnit().sampleToSurfaceArchimedes(
		sampleFlow.flow2D());

	out_sample->position = out_sample->normal.mul(m_radius);

	// FIXME: able to specify mapper channel
	const UvwMapper* mapper = m_metadata->getDefaultChannel().getMapper();
	PH_ASSERT(mapper);
	mapper->positionToUvw(out_sample->position, &out_sample->uvw);

	// FIXME: assumed uniform PDF
	out_sample->pdf = this->PSphere::calcPositionSamplePdfA(out_sample->position);
}

real PSphere::calcExtendedArea() const
{
	return math::TSphere(m_radius).getArea();
}

}// end namespace ph
