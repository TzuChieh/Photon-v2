#include "Core/Intersectable/PBasicSphere.h"
#include "Common/assertion.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Math/Geometry/TSphere.h"

namespace ph
{

PBasicSphere::PBasicSphere(const real radius)
	: Primitive()
	, m_radius(radius)
	, m_rcpRadius(radius > 0.0_r ? 1.0_r / radius : 0.0_r)
{
	PH_ASSERT_GE(radius, 0.0_r);
}

bool PBasicSphere::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	real hitT;
	if(!math::TSphere(m_radius).isIntersecting(ray.getSegment(), &hitT))
	{
		return false;
	}

	probe.pushBaseHit(this, hitT);
	return true;
}

bool PBasicSphere::mayOverlapVolume(const math::AABB3D& volume) const
{
	return math::TSphere(m_radius).mayOverlapVolume(volume);
}

math::AABB3D PBasicSphere::calcAABB() const
{
	return math::TSphere(m_radius).getAABB();
}

real PBasicSphere::calcExtendedArea() const
{
	return math::TSphere(m_radius).getArea();
}

}// end namespace ph
