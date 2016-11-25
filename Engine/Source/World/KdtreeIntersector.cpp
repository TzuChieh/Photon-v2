#include "World/KdtreeIntersector.h"
#include "Model/Geometry/Triangle.h"

#include <iostream>

namespace ph
{

KdtreeIntersector::~KdtreeIntersector() = default;

void KdtreeIntersector::construct(const std::vector<Triangle>& triangles)
{
	std::vector<const Triangle*> trigPtrs;
	for(const auto& triangle : triangles)
	{
		trigPtrs.push_back(&triangle);
	}

	m_rootKdtreeNode.buildTree(trigPtrs);
}

bool KdtreeIntersector::isIntersecting(const Ray& ray, Intersection* out_intersection) const
{
	//std::cout << "isIntersecting" << std::endl;
	bool isFound = m_rootKdtreeNode.findClosestIntersection(ray, out_intersection);
	//std::cout << "found? " << isFound << std::endl;

	return isFound;
}

}// end namespace ph