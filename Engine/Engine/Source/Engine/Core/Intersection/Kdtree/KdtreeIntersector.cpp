#include "Engine/Core/Intersection/Kdtree/KdtreeIntersector.h"
#include "Engine/Core/HitEvent.h"

#include <Common/assertion.h>

//#define KDTREE_INITIAL_TRIANGLE_BUFFER_SIZE 1000000

namespace ph
{

KdtreeIntersector::KdtreeIntersector() : 
	m_nodeIntersectableBuffer(), m_rootKdtreeNode(&m_nodeIntersectableBuffer)// FIXME: rely on init ordering is dangerous
{}

void KdtreeIntersector::update(TSpanView<const Intersectable*> intersectables)
{
	std::vector<const Intersectable*> treeIntersectables;
	for(const auto& intersectable : intersectables)
	{
		treeIntersectables.push_back(intersectable);
	}

	m_rootKdtreeNode.buildTree(treeIntersectables);
}

bool KdtreeIntersector::isIntersecting(HitEvent& event) const
{
	return m_rootKdtreeNode.findClosestIntersection(event);
}

math::AABB3D KdtreeIntersector::calcAABB() const
{
	const KdtreeAABB& kdtreeAABB = m_rootKdtreeNode.getAABB();

	math::AABB3D aabb;
	kdtreeAABB.getAABB(&aabb);

	return aabb;
}

}// end namespace ph
