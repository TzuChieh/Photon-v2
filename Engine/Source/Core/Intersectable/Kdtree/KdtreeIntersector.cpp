#include "Core/Intersectable/Kdtree/KdtreeIntersector.h"
#include "Actor/CookedActorStorage.h"
#include "Core/IntersectionProbe.h"
#include "Core/IntersectionDetail.h"

#include <iostream>

//#define KDTREE_INITIAL_TRIANGLE_BUFFER_SIZE 1000000

namespace ph
{

KdtreeIntersector::KdtreeIntersector() : 
	m_nodeIntersectableBuffer(), m_rootKdtreeNode(&m_nodeIntersectableBuffer)// FIXME: rely on init ordering is dangerous
{

}

KdtreeIntersector::~KdtreeIntersector() = default;

void KdtreeIntersector::update(const CookedActorStorage& cookedActors)
{
	std::vector<const Intersectable*> intersectables;
	for(const auto& primitive : cookedActors.intersectables())
	{
		intersectables.push_back(primitive.get());
	}

	m_rootKdtreeNode.buildTree(intersectables);
}

bool KdtreeIntersector::isIntersecting(const Ray& ray, IntersectionProbe* const out_probe) const
{
	return m_rootKdtreeNode.findClosestIntersection(ray, out_probe);
}

}// end namespace ph