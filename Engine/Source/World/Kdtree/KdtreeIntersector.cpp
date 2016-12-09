#include "World/Kdtree/KdtreeIntersector.h"
#include "Model/Geometry/Triangle.h"

#include <iostream>

//#define KDTREE_INITIAL_TRIANGLE_BUFFER_SIZE 1000000

namespace ph
{

KdtreeIntersector::KdtreeIntersector() : 
	m_nodeTriangleBuffer(), m_rootKdtreeNode(&m_nodeTriangleBuffer)
{

}

KdtreeIntersector::~KdtreeIntersector() = default;

void KdtreeIntersector::construct()
{
	std::vector<const Triangle*> trigPtrs;
	for(const auto& triangle : m_triangles)
	{
		trigPtrs.push_back(&triangle);
	}

	m_rootKdtreeNode.buildTree(trigPtrs);
}

bool KdtreeIntersector::isIntersecting(const Ray& ray, Intersection* out_intersection) const
{
	return m_rootKdtreeNode.findClosestIntersection(ray, out_intersection);
}

}// end namespace ph