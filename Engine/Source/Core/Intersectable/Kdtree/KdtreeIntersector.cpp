#include "Core/Intersectable/Kdtree/KdtreeIntersector.h"
#include "Actor/CookedDataStorage.h"
#include "Core/HitProbe.h"
#include "Common/assertion.h"

#include <iostream>

//#define KDTREE_INITIAL_TRIANGLE_BUFFER_SIZE 1000000

namespace ph
{

KdtreeIntersector::KdtreeIntersector() : 
	m_nodeIntersectableBuffer(), m_rootKdtreeNode(&m_nodeIntersectableBuffer)// FIXME: rely on init ordering is dangerous
{}

void KdtreeIntersector::update(const CookedDataStorage& cookedActors)
{
	std::vector<const Intersectable*> intersectables;
	for(const auto& intersectable : cookedActors.intersectables())
	{
		// HACK
		if(!intersectable->calcAABB().isFiniteVolume())
		{
			continue;
		}

		intersectables.push_back(intersectable.get());
	}

	m_rootKdtreeNode.buildTree(intersectables);
}

bool KdtreeIntersector::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	return m_rootKdtreeNode.findClosestIntersection(ray, probe);
}

math::AABB3D KdtreeIntersector::calcAABB() const
{
	const KdtreeAABB& kdtreeAABB = m_rootKdtreeNode.getAABB();

	math::AABB3D aabb;
	kdtreeAABB.getAABB(&aabb);

	return aabb;
}

}// end namespace ph
