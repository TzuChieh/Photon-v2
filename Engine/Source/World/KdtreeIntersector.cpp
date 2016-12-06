#include "World/KdtreeIntersector.h"
#include "Model/Geometry/Triangle.h"

#include <iostream>

namespace ph
{

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