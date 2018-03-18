#include "Core/Intersectable/PSphere.h"
#include "Common/assertion.h"
#include "Math/constant.h"
#include "Core/Bound/AABB3D.h"
#include "Math/Math.h"
#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/UvwMapper/UvwMapper.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace ph
{

PSphere::PSphere(const PrimitiveMetadata* const metadata, const real radius) : 
	Primitive(metadata),
	m_radius(radius)
{
	PH_ASSERT(radius > 0.0_r);
}

PSphere::~PSphere() = default;

bool PSphere::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	// ray origin:         o
	// ray direction:      d
	// sphere center:      c
	// sphere radius:      r
	// intersection point: p
	// vector dot:         *
	// ray equation:       o + td (t is a scalar variable)
	//
	// To find the intersection point, the length of vector (td - oc) must equals r.
	// This is equivalent to (td - oc)*(td - oc) = r^2. After reformatting, we have
	//
	//              t^2(d*d) - 2t(d*op) + (oc*oc) - r^2 = 0     --- (1)
	//
	// Solving equation (1) for t will yield the intersection point (o + td).

	// vector from ray origin to sphere center
	//
	const Vector3R& oc = Vector3R(0, 0, 0).sub(ray.getOrigin());

	const real a = ray.getDirection().dot(ray.getDirection());// a in equation (1)
	const real b = ray.getDirection().dot(oc);                // b in equation (1) (-2 is cancelled while solving t)
	const real c = oc.dot(oc) - m_radius * m_radius;          // c in equation (1)

	real D = b * b - a * c;
	if(D < 0.0_r)
	{
		return false;
	}
	else
	{
		D = std::sqrt(D);

		const real reciA = 1.0_r / a;

		// pick the closest point in front of ray tail
		// t = (b +- D) / a
		//
		real t = 0.0_r;
		if((t = (b - D) * reciA) > ray.getMinT())
		{
			probe.pushBaseHit(this, t);
			return true;
		}
		else if((t = (b + D) * reciA) > ray.getMinT())
		{
			probe.pushBaseHit(this, t);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void PSphere::calcIntersectionDetail(
	const Ray&       ray, 
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	PH_ASSERT(m_metadata != nullptr);
	const UvwMapper* mapper = m_metadata->getChannel(probe.getChannel()).getMapper();

	const Vector3R& hitPosition = ray.getOrigin().add(ray.getDirection().mul(probe.getHitRayT()));
	const Vector3R& hitNormal   = hitPosition.normalize();

	PH_ASSERT(mapper != nullptr);
	Vector3R hitUvw;
	mapper->map(hitPosition, &hitUvw);

	out_detail->getHitInfo(ECoordSys::LOCAL).setAttributes(
		hitPosition, 
		hitNormal,
		hitNormal,
		probe.getHitRayT());

	/*Vector3R dPdU(0.0_r), dPdV(0.0_r);
	Vector3R dNdU(0.0_r), dNdV(0.0_r);
	const Vector2R dUVab(m_uvwB.x - m_uvwA.x, m_uvwB.y - m_uvwA.y);
	const Vector2R dUVac(m_uvwC.x - m_uvwA.x, m_uvwC.y - m_uvwA.y);
	const real uvDet = dUVab.x * dUVac.y - dUVab.y * dUVac.x;
	if(uvDet != 0.0_r)
	{
		const real reciUvDet = 1.0_r / uvDet;

		dPdU = m_eAB.mul(dUVac.y).add(m_eAC.mul(-dUVab.y)).mulLocal(reciUvDet);
		dPdV = m_eAB.mul(-dUVac.x).add(m_eAC.mul(dUVab.x)).mulLocal(reciUvDet);

		const Vector3R& dNab = m_nB.sub(m_nA);
		const Vector3R& dNac = m_nC.sub(m_nA);
		dNdU = dNab.mul(dUVac.y).add(dNac.mul(-dUVab.y)).mulLocal(reciUvDet);
		dNdV = dNab.mul(-dUVac.x).add(dNac.mul(dUVab.x)).mulLocal(reciUvDet);
	}
	
	out_detail->getHitInfo(ECoordSys::LOCAL).setDerivatives(
		dPdU, dPdV, dNdU, dNdV);*/

	out_detail->getHitInfo(ECoordSys::WORLD) = out_detail->getHitInfo(ECoordSys::LOCAL);
	out_detail->setMisc(this, hitUvw);
}

// Intersection test for solid box and hollow sphere.
// Reference: Jim Arvo's algorithm in Graphics Gems 2
//
bool PSphere::isIntersectingVolumeConservative(const AABB3D& volume) const
{
	const real radius2 = Math::squared(m_radius);

	// These variables are gonna store minimum and maximum squared distances 
	// from the sphere's center to the AABB volume.
	//
	real minDist2 = 0.0_r;
	real maxDist2 = 0.0_r;

	real a, b;

	a = Math::squared(volume.getMinVertex().x);
	b = Math::squared(volume.getMaxVertex().x);
	maxDist2 += std::max(a, b);
	if     (0.0_r < volume.getMinVertex().x) minDist2 += a;
	else if(0.0_r > volume.getMaxVertex().x) minDist2 += b;

	a = Math::squared(volume.getMinVertex().y);
	b = Math::squared(volume.getMaxVertex().y);
	maxDist2 += std::max(a, b);
	if     (0.0_r < volume.getMinVertex().y) minDist2 += a;
	else if(0.0_r > volume.getMaxVertex().y) minDist2 += b;

	a = Math::squared(volume.getMinVertex().z);
	b = Math::squared(volume.getMaxVertex().z);
	maxDist2 += std::max(a, b);
	if     (0.0_r < volume.getMinVertex().z) minDist2 += a;
	else if(0.0_r > volume.getMaxVertex().z) minDist2 += b;

	return minDist2 <= radius2 && radius2 <= maxDist2;
}

void PSphere::calcAABB(AABB3D* const out_aabb) const
{
	out_aabb->setMinVertex(Vector3R(-m_radius, -m_radius, -m_radius));
	out_aabb->setMaxVertex(Vector3R( m_radius,  m_radius,  m_radius));
	out_aabb->expand(Vector3R(0.0001_r));
}

real PSphere::calcPositionSamplePdfA(const Vector3R& position) const
{
	// TODO
	return 0.0_r;
}

void PSphere::genPositionSample(PositionSample* const out_sample) const
{
	// TODO
}

real PSphere::calcExtendedArea() const
{
	return PH_PI_REAL * m_radius * m_radius;
}

}// end namespace ph