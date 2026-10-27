#include "Engine/Core/Intersection/BruteForceIntersector.h"
#include "Engine/Core/Intersection/Intersectable.h"
#include "Engine/Core/HitEvent.h"
#include "Engine/Core/Ray.h"
#include "Engine/Math/Geometry/TAABB3D.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <limits>

namespace ph
{

void BruteForceIntersector::update(TSpanView<const Intersectable*> intersectables)
{
	m_intersectables.clear();
	m_intersectables.shrink_to_fit();

	for(const auto& intersectable : intersectables)
	{
		m_intersectables.push_back(intersectable);
	}
}

bool BruteForceIntersector::isIntersecting(HitEvent& event) const
{
	const HitEvent originalEvent(event);
	for(const Intersectable* intersectable : m_intersectables)
	{
		HitEvent currentEvent(originalEvent);
		if(intersectable->isIntersecting(currentEvent))
		{
			if(currentEvent.getHitRayT() < event.getHitRayT())
			{
				event.copyHitAndKeepRay(currentEvent);
			}
		}
	}

	return event.getHitRayT() < std::numeric_limits<real>::max();
}

bool BruteForceIntersector::isOccluding(const Ray& ray) const
{
	for(const Intersectable* intersectable : m_intersectables)
	{
		if(intersectable->isOccluding(ray))
		{
			return true;
		}
	}

	return false;
}

math::AABB3D BruteForceIntersector::calcAABB() const
{
	if(m_intersectables.empty())
	{
		return math::AABB3D::makeEmpty();
	}

	math::AABB3D unionedAabb = m_intersectables.front()->calcAABB();
	for(auto intersectable : m_intersectables)
	{
		unionedAabb.unionWith(intersectable->calcAABB());
	}

	return unionedAabb;
}

void BruteForceIntersector::forEachIntersection(const Ray& ray, const HitVisitor& visitor) const
{
	for(const Intersectable* intersectable : m_intersectables)
	{
		PH_ASSERT(intersectable);

		forEachIntersectionInTarget(*intersectable, ray, visitor);
	}
}

bool BruteForceIntersector::supportsForEachIntersection() const
{
	return true;
}

}// end namespace ph
