#include "Core/Intersection/BruteForceIntersector.h"
#include "Core/Intersection/Intersectable.h"
#include "Core/HitProbe.h"
#include "Core/Ray.h"
#include "Math/Geometry/TAABB3D.h"

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

bool BruteForceIntersector::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	HitProbe closestProbe;
	real closestRayHitT = std::numeric_limits<real>::max();

	for(const Intersectable* intersectable : m_intersectables)
	{
		HitProbe currentProbe(probe);
		if(intersectable->isIntersecting(ray, currentProbe))
		{
			if(currentProbe.getHitRayT() < closestRayHitT)
			{
				closestRayHitT = currentProbe.getHitRayT();
				closestProbe   = currentProbe;
			}
		}
	}

	if(closestRayHitT < std::numeric_limits<real>::max())
	{
		probe = closestProbe;
		return true;
	}
	else
	{
		return false;
	}
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

}// end namespace ph
