#include "World/Kdtree/KdtreeIntersector.h"
#include "Core/Primitive/PrimitiveStorage.h"

#include <iostream>

//#define KDTREE_INITIAL_TRIANGLE_BUFFER_SIZE 1000000

namespace ph
{

KdtreeIntersector::KdtreeIntersector() : 
	m_nodePrimitiveBuffer(), m_rootKdtreeNode(&m_nodePrimitiveBuffer)// FIXME: rely on init ordering is dangerous
{

}

KdtreeIntersector::~KdtreeIntersector() = default;

void KdtreeIntersector::update(const PrimitiveStorage& primitives)
{
	std::vector<const Primitive*> primPtrs;
	for(const auto& primitive : primitives)
	{
		primPtrs.push_back(primitive.get());
	}

	m_rootKdtreeNode.buildTree(primPtrs);
}

bool KdtreeIntersector::isIntersecting(const Ray& ray, Intersection* out_intersection) const
{
	return m_rootKdtreeNode.findClosestIntersection(ray, out_intersection);
}

}// end namespace ph