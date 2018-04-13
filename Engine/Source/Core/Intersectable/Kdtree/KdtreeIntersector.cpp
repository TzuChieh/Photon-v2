#include "Core/Intersectable/Kdtree/KdtreeIntersector.h"
#include "Actor/CookedDataStorage.h"
#include "Core/HitProbe.h"

#include <iostream>

//#define KDTREE_INITIAL_TRIANGLE_BUFFER_SIZE 1000000

namespace ph
{

KdtreeIntersector::KdtreeIntersector() : 
	m_nodeIntersectableBuffer(), m_rootKdtreeNode(&m_nodeIntersectableBuffer)// FIXME: rely on init ordering is dangerous
{

}

KdtreeIntersector::~KdtreeIntersector() = default;

void KdtreeIntersector::update(const CookedDataStorage& cookedActors)
{
	std::vector<const Intersectable*> intersectables;
	for(const auto& primitive : cookedActors.intersectables())
	{
		intersectables.push_back(primitive.get());
	}

	m_rootKdtreeNode.buildTree(intersectables);
}

bool KdtreeIntersector::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	return m_rootKdtreeNode.findClosestIntersection(ray, probe);
}

}// end namespace ph