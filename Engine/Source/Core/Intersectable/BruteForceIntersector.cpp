#include "Core/Intersectable/BruteForceIntersector.h"
#include "Common/primitive_type.h"
#include "Core/HitProbe.h"
#include "Core/Ray.h"
#include "Core/Intersectable/Intersectable.h"
#include "Actor/CookedDataStorage.h"
#include "Math/Geometry/TAABB3D.h"

#include <limits>

namespace ph
{

BruteForceIntersector::~BruteForceIntersector() = default;

void BruteForceIntersector::update(const CookedDataStorage& cookedActors)
{
	m_intersectables.clear();
	m_intersectables.shrink_to_fit();

	for(const auto& intersectable : cookedActors.intersectables())
	{
		// HACK
		if(!intersectable->calcAABB().isFiniteVolume())
		{
			continue;
		}

		m_intersectables.push_back(intersectable.get());
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
		// FIXME: return an invalid one or a point?
		return math::AABB3D();
	}

	math::AABB3D unionedAabb = m_intersectables.front()->calcAABB();
	for(auto intersectable : m_intersectables)
	{
		unionedAabb.unionWith(intersectable->calcAABB());
	}

	return unionedAabb;
}

}// end namespace ph
