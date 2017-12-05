#include "Core/Intersectable/BruteForceIntersector.h"
#include "Common/primitive_type.h"
#include "Core/HitProbe.h"
#include "Core/Ray.h"
#include "Core/Intersectable/Intersectable.h"
#include "Actor/CookedActorStorage.h"
#include "Core/Bound/AABB3D.h"

#include <limits>

namespace ph
{

BruteForceIntersector::~BruteForceIntersector() = default;

void BruteForceIntersector::update(const CookedActorStorage& cookedActors)
{
	m_intersectables.clear();
	m_intersectables.shrink_to_fit();

	for(const auto& intersectable : cookedActors.intersectables())
	{
		m_intersectables.push_back(intersectable.get());
	}
}

bool BruteForceIntersector::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	HitProbe currentProbe;
	real closestRayHitT = std::numeric_limits<real>::infinity();

	for(const Intersectable* intersectable : m_intersectables)
	{
		currentProbe.clear();
		if(intersectable->isIntersecting(ray, currentProbe))
		{
			if(currentProbe.getHitRayT() < closestRayHitT)
			{
				closestRayHitT = currentProbe.getHitRayT();
				probe          = currentProbe;
			}
		}
	}

	return closestRayHitT != std::numeric_limits<real>::infinity();
}

bool BruteForceIntersector::isIntersecting(const Ray& ray) const
{
	for(const Intersectable* intersectable : m_intersectables)
	{
		if(intersectable->isIntersecting(ray))
		{
			return true;
		}
	}

	return false;
}

void BruteForceIntersector::calcAABB(AABB3D* const out_aabb) const
{
	if(m_intersectables.empty())
	{
		*out_aabb = AABB3D();
		return;
	}

	m_intersectables.front()->calcAABB(out_aabb);
	for(auto intersectable : m_intersectables)
	{
		AABB3D aabb;
		intersectable->calcAABB(&aabb);
		out_aabb->unionWith(aabb);
	}
}

}// end namespace ph