#include "Engine/Core/Intersection/PBasicSphere.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/HitEvent.h"
#include "Engine/Math/Geometry/TSphere.h"

#include <Common/assertion.h>

namespace ph
{

PBasicSphere::PBasicSphere(const real radius)
	: Primitive()
	, m_radius(radius)
	, m_rcpRadius(radius > 0.0_r ? 1.0_r / radius : 0.0_r)
{
	PH_ASSERT_GE(radius, 0.0_r);
}

bool PBasicSphere::isIntersecting(HitEvent& event) const
{
	real hitT;
	if(!math::TSphere(m_radius).isIntersecting(event.getRay().getSegment(), &hitT))
	{
		return false;
	}

	event.pushBaseHit(this, hitT);
	return true;
}

bool PBasicSphere::reintersect(HitEvent& event, HitEvent& srcEvent) const
{
	srcEvent.popHit();

	return PBasicSphere::isIntersecting(event);
}

bool PBasicSphere::isOccluding(const Ray& ray) const
{
	return math::TSphere(m_radius).isIntersecting(ray.getSegment());
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
