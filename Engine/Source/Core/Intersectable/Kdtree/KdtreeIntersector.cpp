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

KdtreeIntersector::~KdtreeIntersector() = default;

void KdtreeIntersector::update(const CookedDataStorage& cookedActors)
{
	std::vector<const Intersectable*> intersectables;
	for(const auto& intersectable : cookedActors.intersectables())
	{
		// HACK
		math::AABB3D aabb;
		intersectable->calcAABB(&aabb);
		if(!aabb.isFiniteVolume())
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

void KdtreeIntersector::calcAABB(math::AABB3D* const out_aabb) const
{
	PH_ASSERT(out_aabb);

	const KdtreeAABB& kdtreeAABB = m_rootKdtreeNode.getAABB();
	kdtreeAABB.getAABB(out_aabb);
}

}// end namespace ph
